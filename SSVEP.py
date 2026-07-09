# -*- coding: utf-8 -*-
import time
import threading
import re
import queue
import socket
import struct
import subprocess
import sys
from collections import deque, Counter

import numpy as np
import pygame

# ============================================================
# 无线数据接收配置
# ============================================================

FORWARD_PORT = 41003                 # 数据转发端口
SPECTRUM_BINS = 128                  # 频谱片段（未使用，保留兼容）

# 帧协议常量
FRAME_CHAR = 0x7E
ESCAPE_CHAR = 0x7D
ESCAPE_XOR = 0x20
CMD_RAW_WAVE = 0x04
CMD_FILT_WAVE = 0x10

# ============================================================
# 自动获取默认网关
# ============================================================

def get_default_gateway():
    """自动获取系统默认网关 IP，失败则返回默认地址"""
    system = sys.platform
    try:
        if system.startswith('win'):
            out = subprocess.check_output("route print -4", shell=True, text=True)
            for line in out.splitlines():
                if '0.0.0.0' in line and '224.0.0.0' not in line:
                    parts = line.split()
                    if len(parts) >= 3:
                        gateway = parts[2]
                        if gateway != '0.0.0.0':
                            return gateway
        else:
            out = subprocess.check_output("ip route show default", shell=True, text=True)
            match = re.search(r'default via ([\d.]+)', out)
            if match:
                return match.group(1)
    except Exception:
        pass
    return "192.168.43.1"            # fallback

# ============================================================
# SSVEP 刺激与评估集成窗口配置
# ============================================================

SCREEN_REFRESH_HZ = 240.0
STIM_FPS = int(SCREEN_REFRESH_HZ)

# ============================================================
# 目标频率（可切换）
# ============================================================

TARGET_FREQS = [11.0, 13.0, 15.0, 17.0]

# ============================================================
# EEG 信号处理参数（无线设备输出为电压值 mV）
# ============================================================

SAMPLE_RATE = 250.0                  # 采样率，需与实际设备一致

NOTCH_FREQ = 50.0
RHO = 0.98
MA_WINDOW = 3

PEAK_WINDOW = 200

USER_GAIN = 1.0
GAIN_LOCK = threading.Lock()
ENABLE_GAIN_INPUT_THREAD = False

DT_HISTORY = 100

# ============================================================
# FBCCA 参数
# ============================================================

FBCCA_WINDOW_SEC = 5.0
FBCCA_WINDOW_SIZE = int(SAMPLE_RATE * FBCCA_WINDOW_SEC)
FBCCA_STEP_SEC = 2.0
FBCCA_STEP_SIZE = int(SAMPLE_RATE * FBCCA_STEP_SEC)
FBCCA_HARMONICS = 2
FBCCA_USE_CHANNELS = [0]            # 使用通道 0 (Oz)

FILTER_BANKS = [
    (6.0, 45.0),
    (10.0, 45.0),
    (14.0, 45.0),
    (18.0, 45.0),
]

ENABLE_FREQ_TOLERANCE_SEARCH = True
FREQ_SEARCH_OFFSETS = [-0.05, 0.0, 0.05]

ENABLE_TIME_WEIGHTED_CCA = True
TIME_WEIGHT_MODE = "tukey"
TIME_WEIGHT_TUKEY_ALPHA = 0.25
TIME_WEIGHT_RECENT_MIN = 0.70
TIME_WEIGHT_RECENT_MAX = 1.10

FBCCA_RATIO_THRESHOLD = 1.25
FBCCA_MARGIN_THRESHOLD = 0.01
FBCCA_MIN_SCORE = 0.00

REFERENCE_CACHE = {}

ENABLE_VOTE = True
VOTE_HISTORY_LEN = 5
VOTE_MIN_COUNT = 3

ENABLE_CHANNEL_QUALITY_CHECK = True
FBCCA_MIN_VALID_CHANNELS = 1
CHANNEL_MIN_STD_MV = 1e-7
CHANNEL_MAX_STD_MV = 100.0
CHANNEL_MAX_PTP_MV = 300.0

# ============================================================
# 50 Hz 陷波 + 显示平滑
# ============================================================

w0 = 2 * np.pi * NOTCH_FREQ / SAMPLE_RATE
cos_w0 = np.cos(w0)

b_notch = [1, -2 * cos_w0, 1]
a_notch = [1, -2 * RHO * cos_w0, RHO ** 2]


def moving_average(samples, new_val):
    samples.append(new_val)
    return np.mean(samples)


class NotchFilter:
    def __init__(self):
        self.x_state = [0.0, 0.0]
        self.y_state = [0.0, 0.0]

    def process(self, x):
        y = (
            b_notch[0] * x
            + b_notch[1] * self.x_state[0]
            + b_notch[2] * self.x_state[1]
            - a_notch[1] * self.y_state[0]
            - a_notch[2] * self.y_state[1]
        )
        self.x_state[1] = self.x_state[0]
        self.x_state[0] = x
        self.y_state[1] = self.y_state[0]
        self.y_state[0] = y
        return y


class DisplayFilter:
    def __init__(self):
        self.notch = NotchFilter()
        self.ma_samples = deque(maxlen=MA_WINDOW)

    def process(self, x):
        y_notch = self.notch.process(x)
        y_smooth = moving_average(self.ma_samples, y_notch)
        return y_smooth


# ============================================================
# CCA / FBCCA 核心函数
# ============================================================

def make_fbcca_weights(num_banks):
    weights = []
    for i in range(num_banks):
        m = i + 1
        weights.append(m ** (-1.25) + 0.25)
    return np.array(weights, dtype=float)


def generate_reference_signals(freq, sample_rate, n_samples, n_harmonics, band_low, band_high):
    t = np.arange(n_samples) / sample_rate
    refs = []
    nyquist = sample_rate / 2.0
    for h in range(1, n_harmonics + 1):
        harmonic_freq = h * freq
        if harmonic_freq < band_low or harmonic_freq > band_high or harmonic_freq >= nyquist:
            continue
        refs.append(np.sin(2 * np.pi * harmonic_freq * t))
        refs.append(np.cos(2 * np.pi * harmonic_freq * t))
    if len(refs) == 0:
        return None
    return np.array(refs, dtype=float)


def get_reference_signals_cached(freq, sample_rate, n_samples, n_harmonics, band_low, band_high):
    key = (
        round(float(freq), 6),
        round(float(sample_rate), 6),
        int(n_samples),
        int(n_harmonics),
        round(float(band_low), 6),
        round(float(band_high), 6),
    )
    if key in REFERENCE_CACHE:
        return REFERENCE_CACHE[key]
    reference = generate_reference_signals(freq, sample_rate, n_samples, n_harmonics, band_low, band_high)
    REFERENCE_CACHE[key] = reference
    return reference


def inv_sqrtm_psd(matrix, eps=1e-8):
    eigenvalues, eigenvectors = np.linalg.eigh(matrix)
    eigenvalues = np.maximum(eigenvalues, eps)
    inv_sqrt_values = 1.0 / np.sqrt(eigenvalues)
    inv_sqrt_matrix = eigenvectors @ np.diag(inv_sqrt_values) @ eigenvectors.T
    return inv_sqrt_matrix


def tukey_window(n_samples, alpha=0.25):
    n_samples = int(n_samples)
    if n_samples <= 0:
        return np.array([], dtype=float)
    if n_samples == 1:
        return np.ones(1, dtype=float)
    alpha = float(np.clip(alpha, 0.0, 1.0))
    if alpha <= 0.0:
        return np.ones(n_samples, dtype=float)
    if alpha >= 1.0:
        return np.hanning(n_samples)
    n = np.arange(n_samples, dtype=float)
    w = np.ones(n_samples, dtype=float)
    edge = alpha * (n_samples - 1) / 2.0
    left = n < edge
    right = n >= (n_samples - 1) * (1.0 - alpha / 2.0)
    if np.any(left):
        w[left] = 0.5 * (1.0 + np.cos(np.pi * (2.0 * n[left] / (alpha * (n_samples - 1)) - 1.0)))
    if np.any(right):
        w[right] = 0.5 * (1.0 + np.cos(np.pi * (2.0 * n[right] / (alpha * (n_samples - 1)) - 2.0 / alpha + 1.0)))
    return w


def make_time_weights(n_samples):
    if not ENABLE_TIME_WEIGHTED_CCA:
        return None
    n_samples = int(n_samples)
    if n_samples <= 0:
        return None
    mode = str(TIME_WEIGHT_MODE).lower().strip()
    if mode in ("none", "flat", "rect", "rectangle"):
        w = np.ones(n_samples, dtype=float)
    elif mode == "hann":
        w = np.hanning(n_samples)
    elif mode == "tukey":
        w = tukey_window(n_samples, alpha=TIME_WEIGHT_TUKEY_ALPHA)
    elif mode == "recent_linear":
        w = np.linspace(TIME_WEIGHT_RECENT_MIN, TIME_WEIGHT_RECENT_MAX, n_samples, dtype=float)
    elif mode == "tukey_recent":
        base = tukey_window(n_samples, alpha=TIME_WEIGHT_TUKEY_ALPHA)
        recent = np.linspace(TIME_WEIGHT_RECENT_MIN, TIME_WEIGHT_RECENT_MAX, n_samples, dtype=float)
        w = base * recent
    else:
        w = np.ones(n_samples, dtype=float)
    w = np.asarray(w, dtype=float)
    w[~np.isfinite(w)] = 0.0
    w = np.maximum(w, 1e-8)
    mean_w = np.mean(w)
    if mean_w <= 0:
        return None
    return w / mean_w


def weighted_center(X, sample_weights):
    if sample_weights is None:
        return X - np.mean(X, axis=1, keepdims=True)
    w = np.asarray(sample_weights, dtype=float)
    w = w / (np.sum(w) + 1e-12)
    mean = X @ w
    return X - mean[:, np.newaxis]


def cca_corrcoef(X, Y, reg=1e-6, sample_weights=None):
    X = np.asarray(X, dtype=float)
    Y = np.asarray(Y, dtype=float)
    if X.ndim != 2 or Y.ndim != 2 or X.shape[1] != Y.shape[1]:
        return 0.0
    n_samples = X.shape[1]
    if n_samples < 5:
        return 0.0
    if sample_weights is None:
        X = X - np.mean(X, axis=1, keepdims=True)
        Y = Y - np.mean(Y, axis=1, keepdims=True)
        Cxx = (X @ X.T) / (n_samples - 1)
        Cyy = (Y @ Y.T) / (n_samples - 1)
        Cxy = (X @ Y.T) / (n_samples - 1)
    else:
        w = np.asarray(sample_weights, dtype=float).reshape(-1)
        if w.shape[0] != n_samples or not np.all(np.isfinite(w)):
            return 0.0
        w = np.maximum(w, 0.0)
        weight_sum = np.sum(w)
        if weight_sum <= 1e-12:
            return 0.0
        w = w / weight_sum
        X = weighted_center(X, w)
        Y = weighted_center(Y, w)
        sqrt_w = np.sqrt(w)[np.newaxis, :]
        Xw = X * sqrt_w
        Yw = Y * sqrt_w
        Cxx = Xw @ Xw.T
        Cyy = Yw @ Yw.T
        Cxy = Xw @ Yw.T
    Cxx += reg * np.eye(Cxx.shape[0])
    Cyy += reg * np.eye(Cyy.shape[0])
    Cxx_inv_sqrt = inv_sqrtm_psd(Cxx)
    Cyy_inv_sqrt = inv_sqrtm_psd(Cyy)
    M = Cxx_inv_sqrt @ Cxy @ Cyy_inv_sqrt
    singular_values = np.linalg.svd(M, compute_uv=False)
    if len(singular_values) == 0:
        return 0.0
    rho = float(np.clip(singular_values[0], 0.0, 1.0))
    return rho


def fft_bandpass_eeg(eeg_data, sample_rate, low_freq, high_freq):
    eeg_data = np.asarray(eeg_data, dtype=float)
    n_samples = eeg_data.shape[1]
    freqs = np.fft.rfftfreq(n_samples, d=1.0 / sample_rate)
    spectrum = np.fft.rfft(eeg_data, axis=1)
    mask = (freqs >= low_freq) & (freqs <= high_freq)
    spectrum = spectrum * mask[np.newaxis, :]
    filtered = np.fft.irfft(spectrum, n=n_samples, axis=1)
    return filtered


def preprocess_eeg_for_fbcca(eeg_data, sample_rate, low_freq, high_freq):
    X = np.asarray(eeg_data, dtype=float)
    X = X - np.mean(X, axis=1, keepdims=True)
    X = fft_bandpass_eeg(X, sample_rate, low_freq, high_freq)
    X = X - np.mean(X, axis=1, keepdims=True)
    std = np.std(X, axis=1, keepdims=True)
    X = X / (std + 1e-8)
    return X


def fbcca_predict(eeg_data, target_freqs, sample_rate, n_harmonics, filter_banks):
    eeg_data = np.asarray(eeg_data, dtype=float)
    n_samples = eeg_data.shape[1]
    weights = make_fbcca_weights(len(filter_banks))
    sample_weights = make_time_weights(n_samples)

    preprocessed_banks = []
    for low_freq, high_freq in filter_banks:
        X_bank = preprocess_eeg_for_fbcca(eeg_data, sample_rate, low_freq, high_freq)
        preprocessed_banks.append(X_bank)

    scores = {}
    bank_score_details = {}
    best_ref_freqs = {}

    search_offsets = FREQ_SEARCH_OFFSETS if ENABLE_FREQ_TOLERANCE_SEARCH else [0.0]

    for target_freq in target_freqs:
        best_total_score = -1.0
        best_bank_scores = None
        best_ref_freq = target_freq
        for offset in search_offsets:
            ref_freq = target_freq + offset
            if ref_freq <= 0:
                continue
            total_score = 0.0
            freq_bank_scores = []
            for bank_idx, (low_freq, high_freq) in enumerate(filter_banks):
                X_bank = preprocessed_banks[bank_idx]
                reference = get_reference_signals_cached(
                    freq=ref_freq,
                    sample_rate=sample_rate,
                    n_samples=n_samples,
                    n_harmonics=n_harmonics,
                    band_low=low_freq,
                    band_high=high_freq
                )
                if reference is None:
                    rho = 0.0
                else:
                    rho = cca_corrcoef(X_bank, reference, sample_weights=sample_weights)
                bank_score = rho ** 2
                weighted_score = weights[bank_idx] * bank_score
                total_score += weighted_score
                freq_bank_scores.append(bank_score)
            if total_score > best_total_score:
                best_total_score = total_score
                best_bank_scores = freq_bank_scores
                best_ref_freq = ref_freq
        scores[target_freq] = float(max(best_total_score, 0.0))
        bank_score_details[target_freq] = best_bank_scores
        best_ref_freqs[target_freq] = best_ref_freq

    predicted_freq = max(scores, key=scores.get)
    return predicted_freq, scores, bank_score_details, best_ref_freqs


def choose_fbcca_result(scores, ratio_threshold, min_score=0.0, margin_threshold=0.0):
    if not scores:
        return None, 0.0, 0.0, 0.0
    sorted_items = sorted(scores.items(), key=lambda item: item[1], reverse=True)
    best_freq, best_score = sorted_items[0]
    if best_score < min_score:
        return None, 0.0, best_score, 0.0
    if len(sorted_items) == 1:
        return best_freq, 999.0, best_score, best_score
    second_freq, second_score = sorted_items[1]
    ratio = best_score / (second_score + 1e-12)
    margin = best_score - second_score
    if ratio < ratio_threshold or margin < margin_threshold:
        return None, ratio, best_score, margin
    return best_freq, ratio, best_score, margin


# ============================================================
# 质量检测 + 投票
# ============================================================

def is_channel_usable(x):
    x = np.asarray(x, dtype=float)
    if len(x) < 10 or not np.all(np.isfinite(x)):
        return False
    std = np.std(x)
    ptp = np.max(x) - np.min(x)
    if std < CHANNEL_MIN_STD_MV or std > CHANNEL_MAX_STD_MV or ptp > CHANNEL_MAX_PTP_MV:
        return False
    return True


VOTE_HISTORY = deque(maxlen=VOTE_HISTORY_LEN)


def vote_result(new_result):
    if not ENABLE_VOTE:
        return new_result, Counter()
    if new_result is None:
        VOTE_HISTORY.clear()
        return None, Counter()
    VOTE_HISTORY.append(new_result)
    if len(VOTE_HISTORY) < VOTE_MIN_COUNT:
        return None, Counter(VOTE_HISTORY)
    counter = Counter(VOTE_HISTORY)
    if not counter:
        return None, counter
    best_freq, count = counter.most_common(1)[0]
    if count >= VOTE_MIN_COUNT:
        return best_freq, counter
    return None, counter


# ============================================================
# 无线数据接收线程（无 PyQt5 依赖）
# ============================================================

class FrameParser:
    """解析无线数据帧，仅提取波形数据"""
    def __init__(self, data_queue):
        self.data_queue = data_queue
        self.state = 0
        self.payload = bytearray()

    def parse(self, b):
        if self.state == 0:
            if b == FRAME_CHAR:
                self.state = 1
                self.payload = bytearray()
        elif self.state == 1:
            if b == ESCAPE_CHAR:
                self.state = 2
            elif b == FRAME_CHAR:
                if len(self.payload) >= 1:
                    self._process()
                self.payload = bytearray()
            else:
                self.payload.append(b)
        elif self.state == 2:
            self.payload.append(b ^ ESCAPE_XOR)
            self.state = 1

    def _process(self):
        cmd = self.payload[0]
        data = self.payload[1:]
        # 仅处理波形数据包 (CMD_RAW_WAVE 或 CMD_FILT_WAVE)
        if cmd in (CMD_RAW_WAVE, CMD_FILT_WAVE) and len(data) == 8:
            ch0, ch1 = struct.unpack('<ff', data)      # 电压值，单位 V（通常）
            # 无线设备输出可能是 V，转换为 mV 以便与原有系统一致
            self.data_queue.put((ch0 * 1000.0, ch1 * 1000.0))


class DataReceiver(threading.Thread):
    def __init__(self, host, port, data_queue):
        super().__init__(daemon=True)
        self.host = host
        self.port = port
        self.data_queue = data_queue
        self.running = True
        self.status = "未连接"

    def run(self):
        while self.running:
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(1.0)
                sock.connect((self.host, self.port))
                self.status = f"已连接 {self.host}:{self.port}"
                parser = FrameParser(self.data_queue)
                while self.running:
                    try:
                        data = sock.recv(4096)
                        if not data:
                            break
                        for b in data:
                            parser.parse(b)
                    except socket.timeout:
                        continue
                    except Exception as e:
                        print(f"接收错误: {e}")
                        break
            except Exception as e:
                self.status = f"连接失败: {e}，3秒后重试"
                time.sleep(3)
            finally:
                if sock:
                    sock.close()

    def stop(self):
        self.running = False


# ============================================================
# 增益调整线程
# ============================================================

def gain_adjust_thread():
    global USER_GAIN
    while True:
        try:
            new_gain = input("\n请输入新的显示增益系数，例如 2.5 或 0.5，直接回车保持当前值: ")
            if new_gain.strip() == "":
                continue
            gain_val = float(new_gain)
            with GAIN_LOCK:
                USER_GAIN = gain_val
            print(f"显示增益已更新为 {USER_GAIN:.4f}")
        except ValueError:
            print("输入无效，请输入一个数字。")
        except KeyboardInterrupt:
            break


# ============================================================
# 集成全屏窗口：刺激 + FBCCA 结果显示
# ============================================================

def integrated_window(host=None, port=FORWARD_PORT):
    global USER_GAIN

    # ---------- 初始化 Pygame（全屏）----------
    pygame.init()
    try:
        info = pygame.display.Info()
        screen_width = info.current_w
        screen_height = info.current_h
        screen = pygame.display.set_mode((screen_width, screen_height), pygame.FULLSCREEN | pygame.DOUBLEBUF, vsync=1)
    except TypeError:
        screen = pygame.display.set_mode((screen_width, screen_height), pygame.FULLSCREEN | pygame.DOUBLEBUF)
    except Exception:
        screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN | pygame.DOUBLEBUF)
        screen_width, screen_height = screen.get_size()

    pygame.display.set_caption("SSVEP Stimulus + FBCCA Evaluation")
    clock = pygame.time.Clock()
    font_large = pygame.font.SysFont("Arial", int(screen_height * 0.06))
    font_small = pygame.font.SysFont("Arial", int(screen_height * 0.025))

    # ---------- 刺激频率状态 ----------
    current_freq_index = 0
    current_stim_freq = TARGET_FREQS[current_freq_index]

    # ---------- 创建数据队列并启动无线接收线程 ----------
    if host is None:
        host = get_default_gateway()
    print(f"无线数据接收目标: {host}:{port}")
    data_queue = queue.Queue()
    receiver = DataReceiver(host, port, data_queue)
    receiver.start()

    # ---------- 信号处理组件 ----------
    display_filters = [DisplayFilter(), DisplayFilter()]
    analysis_filters = [NotchFilter(), NotchFilter()]
    peak_histories = [deque(maxlen=PEAK_WINDOW) for _ in range(2)]

    fbcca_buffers = [deque(maxlen=FBCCA_WINDOW_SIZE) for _ in range(2)]
    fbcca_total_sample_count = 0
    fbcca_since_last_update = 0
    fbcca_round_index = 0

    # FBCCA 结果变量
    current_fbcca_result = None
    current_raw_fbcca_result = None
    current_fbcca_scores = {}
    current_fbcca_ratio = 0.0
    current_fbcca_best_score = 0.0
    current_fbcca_margin = 0.0
    current_used_channels = []
    current_vote_counter = Counter()

    dt_history = deque(maxlen=DT_HISTORY)
    prev_time = None
    sample_counter = 0

    # ---------- 增益线程 ----------
    if ENABLE_GAIN_INPUT_THREAD:
        threading.Thread(target=gain_adjust_thread, daemon=True).start()

    # ---------- 内部函数：处理一对采样（电压值单位 mV）----------
    def process_sample_pair(ch0_mv, ch1_mv):
        nonlocal prev_time, sample_counter
        nonlocal fbcca_total_sample_count, fbcca_since_last_update
        nonlocal fbcca_round_index
        nonlocal current_fbcca_result, current_raw_fbcca_result, current_fbcca_scores
        nonlocal current_fbcca_ratio, current_fbcca_best_score, current_fbcca_margin
        nonlocal current_used_channels, current_vote_counter

        now = time.time()
        sample_counter += 1

        if prev_time is not None:
            dt = now - prev_time
            dt_history.append(dt)
            if sample_counter % 50 == 0:
                avg_dt = np.mean(dt_history) if dt_history else 0
                estimated_fs = 1.0 / avg_dt if avg_dt > 0 else 0
                filled = min(len(fbcca_buffers[0]), FBCCA_WINDOW_SIZE)
                update_progress = 100.0 * fbcca_since_last_update / FBCCA_STEP_SIZE if FBCCA_STEP_SIZE > 0 else 0
                print(f"\r采样间隔 | Avg: {avg_dt*1000:.2f} ms | Estimated Fs: {estimated_fs:.1f} Hz | "
                      f"FBCCA window: {filled}/{FBCCA_WINDOW_SIZE} | Next update: {update_progress:.1f}%",
                      end="", flush=True)

        prev_time = now

        # 处理两个通道（电压值 mV）
        mv_raw_list = [ch0_mv, ch1_mv]
        for ch_idx, mv_raw in enumerate(mv_raw_list):
            peak_histories[ch_idx].append(mv_raw)
            _ = display_filters[ch_idx].process(mv_raw)      # 仅用于显示（不再绘图）
            mv_analysis = analysis_filters[ch_idx].process(mv_raw)
            fbcca_buffers[ch_idx].append(mv_analysis)

        fbcca_total_sample_count += 1
        fbcca_since_last_update += 1

        # FBCCA 判决
        enough_window = all(len(fbcca_buffers[ch]) >= FBCCA_WINDOW_SIZE for ch in FBCCA_USE_CHANNELS)
        enough_step = fbcca_since_last_update >= FBCCA_STEP_SIZE

        if enough_window and enough_step:
            fbcca_since_last_update = 0
            used_rows = []
            used_channels = []
            for ch in FBCCA_USE_CHANNELS:
                data_ch = np.array(list(fbcca_buffers[ch])[-FBCCA_WINDOW_SIZE:])
                if ENABLE_CHANNEL_QUALITY_CHECK:
                    usable = is_channel_usable(data_ch)
                else:
                    usable = True
                if usable:
                    used_rows.append(data_ch)
                    used_channels.append(ch)
            current_used_channels = used_channels

            if len(used_rows) < FBCCA_MIN_VALID_CHANNELS:
                current_raw_fbcca_result = None
                current_fbcca_result = "UNCERTAIN"
                current_fbcca_scores = {}
                current_fbcca_ratio = 0.0
                current_fbcca_best_score = 0.0
                current_fbcca_margin = 0.0
                return

            eeg_data = np.array(used_rows)
            raw_predicted_freq, raw_scores, bank_details, best_ref_freqs = fbcca_predict(
                eeg_data=eeg_data,
                target_freqs=TARGET_FREQS,
                sample_rate=SAMPLE_RATE,
                n_harmonics=FBCCA_HARMONICS,
                filter_banks=FILTER_BANKS
            )
            chosen_freq, ratio, best_score, margin = choose_fbcca_result(
                scores=raw_scores,
                ratio_threshold=FBCCA_RATIO_THRESHOLD,
                min_score=FBCCA_MIN_SCORE,
                margin_threshold=FBCCA_MARGIN_THRESHOLD
            )
            voted_freq, vote_counter = vote_result(chosen_freq)

            current_raw_fbcca_result = chosen_freq
            current_fbcca_scores = raw_scores
            current_fbcca_ratio = ratio
            current_fbcca_best_score = best_score
            current_fbcca_margin = margin
            current_vote_counter = vote_counter

            if voted_freq is None:
                current_fbcca_result = "UNCERTAIN"
            else:
                current_fbcca_result = voted_freq

            fbcca_round_index += 1
            print(f"\n===== FBCCA Round {fbcca_round_index} =====")
            print(f"Voted: {current_fbcca_result}, Raw: {chosen_freq}, Ratio: {ratio:.2f}, Margin: {margin:.4f}")
            print("Scores:", {f"{k:.2f}": f"{v:.4f}" for k, v in raw_scores.items()})

    # ---------- 绘制信息块至屏幕中心 ----------
    def draw_centered_texts(screen, lines):
        total_height = sum(surf.get_height() for surf, _ in lines)
        start_y = (screen_height - total_height) // 2
        for text_surface, _ in lines:
            text_rect = text_surface.get_rect(center=(screen_width // 2, start_y + text_surface.get_height() // 2))
            screen.blit(text_surface, text_rect)
            start_y += text_surface.get_height()

    # ---------- 主循环 ----------
    start_time = time.perf_counter()
    running = True

    while running:
        # 事件处理
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                elif event.key == pygame.K_UP:
                    current_freq_index = (current_freq_index - 1) % len(TARGET_FREQS)
                    current_stim_freq = TARGET_FREQS[current_freq_index]
                    VOTE_HISTORY.clear()
                    print(f"Stim frequency changed to {current_stim_freq} Hz")
                elif event.key == pygame.K_DOWN:
                    current_freq_index = (current_freq_index + 1) % len(TARGET_FREQS)
                    current_stim_freq = TARGET_FREQS[current_freq_index]
                    VOTE_HISTORY.clear()
                    print(f"Stim frequency changed to {current_stim_freq} Hz")

        # 从无线队列中取出所有波形数据并处理
        while not data_queue.empty():
            try:
                ch0_mv, ch1_mv = data_queue.get_nowait()
                process_sample_pair(ch0_mv, ch1_mv)
            except queue.Empty:
                break

        # 闪烁背景
        elapsed = time.perf_counter() - start_time
        period = 1.0 / current_stim_freq
        half = period / 2.0
        if (elapsed % period) < half:
            bg_color = (255, 255, 255)  # 亮
            text_color = (0, 0, 0)
        else:
            bg_color = (50, 50, 50)      # 暗
            text_color = (255, 255, 255)

        screen.fill(bg_color)

        # ---- 组装居中文字信息 ----
        lines_to_draw = []

        # 连接状态
        status_text = font_small.render(receiver.status, True, text_color)
        lines_to_draw.append((status_text, None))

        # 当前刺激频率
        stim_line = font_large.render(f"Stim: {current_stim_freq:.2f} Hz", True, text_color)
        lines_to_draw.append((stim_line, None))

        # FBCCA 结果
        if current_fbcca_result is not None:
            if current_fbcca_result == "UNCERTAIN":
                res_str = "UNCERTAIN"
                res_color = (200, 0, 0)
            else:
                res_str = f"{current_fbcca_result:.2f} Hz"
                res_color = (0, 200, 0)
            res_text = font_large.render(f"Result: {res_str}", True, res_color)
            lines_to_draw.append((res_text, None))

            # 详细信息
            detail_color = text_color
            details = [
                f"Ratio: {current_fbcca_ratio:.2f}   Margin: {current_fbcca_margin:.4f}",
                f"Best Score: {current_fbcca_best_score:.4f}",
                f"Channels used: {current_used_channels}",
            ]
            score_str = "Scores: " + " | ".join(f"{f:.2f}:{current_fbcca_scores.get(f,0):.4f}" for f in TARGET_FREQS)
            details.append(score_str)

            for det in details:
                det_surf = font_small.render(det, True, detail_color)
                lines_to_draw.append((det_surf, None))
        else:
            wait_text = font_small.render("等待无线数据 ...", True, text_color)
            lines_to_draw.append((wait_text, None))

        # 操作提示（底部居中）
        hint_text = font_small.render("UP/DOWN: 切换频率 | ESC: 退出", True, (150, 150, 150))
        hint_rect = hint_text.get_rect(center=(screen_width // 2, screen_height - 30))
        screen.blit(hint_text, hint_rect)

        # 将信息块居中
        draw_centered_texts(screen, lines_to_draw)

        pygame.display.flip()
        clock.tick_busy_loop(STIM_FPS)

    # 清理
    receiver.stop()
    print("程序退出。")


# ============================================================
# 程序入口
# ============================================================

def main():
    import argparse
    ap = argparse.ArgumentParser(description="SSVEP + FBCCA 系统")
    ap.add_argument("--host", default=None, help="Android医生端IP地址（默认自动获取网关）")
    ap.add_argument("--port", type=int, default=FORWARD_PORT, help="转发端口（默认41003）")
    args = ap.parse_args()

    host = args.host or get_default_gateway()
    print("\n无线版全屏 SSVEP + FBCCA 系统")
    print(f"目标频率: {TARGET_FREQS}")
    print("使用 UP/DOWN 键切换刺激频率，ESC 退出")
    print(f"连接目标: {host}:{args.port}")
    integrated_window(host=host, port=args.port)


if __name__ == "__main__":
    main()