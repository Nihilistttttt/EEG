#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
EEG 训练/测试上位机 - 无线版本（通过Android医生端中转）
- 数据接收：连接Android FORWARD_PORT(41003)，接收MCU转发的原始数据
- 命令发送：连接Android PATIENT_CMD_PORT(41005)，发送命令到MCU
- 41003端口数据为文本行+V1二进制帧混合，本脚本只提取文本行
- 其余训练/测试/停止逻辑与串口版完全一致
"""

import sys
import re
import random
import csv
import json
import math
import socket
import struct
import subprocess
import time
from datetime import datetime
from pathlib import Path
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QTextEdit, QLabel, QComboBox, QGroupBox,
    QProgressBar, QMessageBox, QLineEdit, QPlainTextEdit, QTabWidget
)
from PyQt5.QtCore import QThread, pyqtSignal, Qt, QTimer
from PyQt5.QtGui import QFont, QIcon, QTextCursor

# ========================= 训练参数 =========================
DIR_TRIALS_PER_CLASS = 2
CSP_TRIALS_PER_CLASS = 6
CSP_WINDOWS_PER_TRIAL = 4

FORWARD_PORT = 41003
CMD_PORT = 41005
import subprocess

FORWARD_PORT = 41003
CMD_PORT = 41005


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
    return "192.168.43.1"


# ========================= TCP接收线程 =========================
_VALID_LINE_PREFIXES = (
    'TASK,', 'TASK,DONE', 'READY_TRAIN', 'READY_TEST', 'TASK,STOPPED',
    'DIRCSV,', 'CSP_BEGIN,', 'CSP,', 'CSP_END,',
    'RESULT,', 'MODE_SET_OK,', 'MODEL_SET_OK,', 'MODEL,',
    'STATUS,', 'IPCDIAG,', 'POSTURE,', 'POSTURE_STATE,',
    'POSTURE_ANGLE,', 'PSTATUS,', 'TURN_EVENT,', 'FALL_EVENT,',
    'NO_TURN_ALERT,', 'ERROR', 'RX[', 'V5F_DIAG,', 'V5F_RAW,',
    'MODE_DIAG,', 'DIRCFG,', 'INTENT=',
)


def _is_valid_text_line(line):
    if len(line) < 3:
        return False
    for p in _VALID_LINE_PREFIXES:
        if line.startswith(p):
            return True
    return False


class TcpThread(QThread):
    data_received = pyqtSignal(str)

    MAX_RECV_BUF = 8192

    def __init__(self, ip, data_port=FORWARD_PORT, cmd_port=CMD_PORT):
        super().__init__()
        self.ip = ip
        self.data_port = data_port
        self.cmd_port = cmd_port
        self.data_sock = None
        self.cmd_sock = None
        self.running = False
        self._cmd_broken = False

    def _emit_text(self, text_buf):
        if not text_buf:
            return
        try:
            line = text_buf.decode('utf-8', errors='ignore').strip()
            if line and _is_valid_text_line(line):
                self.data_received.emit(line)
        except Exception:
            pass

    def _connect(self):
        self.data_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.data_sock.settimeout(5)
        self.data_sock.connect((self.ip, self.data_port))
        self.data_sock.settimeout(0.1)

        self.cmd_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.cmd_sock.settimeout(5)
        self.cmd_sock.connect((self.ip, self.cmd_port))
        self.cmd_sock.settimeout(2)
        self._cmd_broken = False

    def run(self):
        while self.running:
            try:
                self._connect()
                self.data_received.emit("TCP_CONNECTED")
                self._recv_loop()
            except Exception as e:
                if self.running:
                    self.data_received.emit(f"TCP_ERROR:{str(e)}")
            finally:
                for s in (self.data_sock, self.cmd_sock):
                    if s:
                        try:
                            s.close()
                        except Exception:
                            pass
                self.data_sock = None
                self.cmd_sock = None

            if self.running:

                time.sleep(3)

    def _recv_loop(self):
        recv_buf = bytearray()
        text_buf = bytearray()

        while self.running:
            try:
                chunk = self.data_sock.recv(4096)
                if not chunk:
                    break
                recv_buf.extend(chunk)

                if len(recv_buf) > self.MAX_RECV_BUF:
                    recv_buf = recv_buf[-4096:]

                while recv_buf:
                    b = recv_buf[0]
                    if b == 0x7E:
                        end_idx = self._find_frame_end(recv_buf)
                        if end_idx < 0:
                            if len(recv_buf) > 1024:
                                recv_buf = recv_buf[1:]
                            break
                        recv_buf = recv_buf[end_idx + 1:]
                    elif b == 0x0A or b == 0x0D:
                        self._emit_text(text_buf)
                        text_buf = bytearray()
                        recv_buf = recv_buf[1:]
                    elif 0x20 <= b < 0x7F:
                        text_buf.append(b)
                        recv_buf = recv_buf[1:]
                    else:
                        text_buf = bytearray()
                        recv_buf = recv_buf[1:]
            except socket.timeout:
                continue
            except Exception as e:
                if self.running:
                    self.data_received.emit(f"TCP_ERROR:{str(e)}")
                break

        self._emit_text(text_buf)

    @staticmethod
    def _find_frame_end(buf):
        i = 1
        escaped = False
        while i < len(buf):
            b = buf[i]
            if escaped:
                escaped = False
            elif b == 0x7D:
                escaped = True
            elif b == 0x7E:
                return i
            i += 1
        return -1

    def stop(self):
        self.running = False
        for s in (self.data_sock, self.cmd_sock):
            if s:
                try:
                    s.close()
                except Exception:
                    pass
        self.quit()
        self.wait()

    def send_command(self, cmd):
        if self._cmd_broken:
            return False
        if not self.cmd_sock:
            self._cmd_broken = True
            return False
        try:
            self.cmd_sock.sendall((cmd + '\r\n').encode('utf-8'))
            return True
        except Exception:
            self._cmd_broken = True
            return False


# ========================= 主窗口 =========================
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("EEG 控制台 - 无线版 (Android中转)")
        self.setGeometry(150, 150, 1500, 850)
        self.setMinimumSize(950, 750)


        self.train_data = []
        self.dir_row_trial_ids = []
        self.csp_data = []
        self.trial_sequence = []
        self.current_trial_index = 0
        self.is_training = False
        self.is_testing = False
        self.waiting_for_ready = False
        self.pending_train = False
        self.pending_test = False
        self.pending_work_command = None
        self.infer_model_select = "AUTO"
        self.test_results = []
        self.received_feature_count = 0

        self.csp_window_buffer = {}
        self.csp_label = 0
        self.csp_win_id = 0
        self.csp_receiving = False
        self.csp_window_trial_map = {}

        self.posture_name = "UNKNOWN"
        self.posture_gx = 0
        self.posture_gy = 0
        self.posture_gz = 0
        self.posture_roll_deg = None
        self.posture_pitch_deg = None
        self.posture_yaw_deg = None
        self.posture_conf = 0
        self.posture_stable_ms = 0
        self.posture_metrics_valid = False
        self.turn_count = 0
        self.fall_detected = False

        self._last_seq = -1

        self.init_ui()
        self.apply_stylesheet()
        self.tcp_thread = None
        self.discover_devices()

    def apply_stylesheet(self):
        style = """
        QMainWindow {
            background-color: #f0f2f5;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #d0d7de;
            border-radius: 8px;
            margin-top: 8px;
            padding-top: 12px;
            background-color: white;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px 0 6px;
            color: #24292f;
        }
        QLabel {
            color: #24292f;
            font-size: 9pt;
        }
        QPushButton {
            background-color: #40739e;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 6px 12px;
            font-weight: bold;
            font-size: 9pt;
        }
        QPushButton:hover {
            background-color: #487eb0;
        }
        QPushButton:pressed {
            background-color: #353b48;
        }
        QPushButton:disabled {
            background-color: #b2bec3;
            color: #dfe6e9;
        }
        QPushButton#trainBtn { background-color: #00a8ff; }
        QPushButton#trainBtn:hover { background-color: #0097e6; }
        QPushButton#testBtn { background-color: #e1b12c; }
        QPushButton#testBtn:hover { background-color: #fbc531; }
        QPushButton#stopBtn { background-color: #e84118; }
        QPushButton#stopBtn:hover { background-color: #c23616; }
        QPushButton#connectBtn { background-color: #4cd137; }
        QPushButton#connectBtn:hover { background-color: #44bd32; }
        QPushButton#statusBtn { background-color: #9c88ff; }
        QPushButton#statusBtn:hover { background-color: #8c7ae6; }
        QPushButton#applyModeBtn { background-color: #00a8ff; }
        QPushButton#applyModeBtn:hover { background-color: #0097e6; }
        QPushButton#ipcdiagBtn { background-color: #4cd137; }
        QPushButton#ipcdiagBtn:hover { background-color: #44bd32; }
        QPushButton#ipcdiagBtn:checked { background-color: #7f8c8d; }
        QPushButton#postureBtn { background-color: #00cec9; }
        QPushButton#postureBtn:hover { background-color: #01b5b0; }
        QPushButton#postureBtn:checked { background-color: #7f8c8d; }

        QLineEdit, QComboBox, QPlainTextEdit, QTextEdit {
            border: 1px solid #d0d7de;
            border-radius: 6px;
            padding: 4px 8px;
            background-color: #f8f9fa;
            font-size: 9pt;
        }
        QLineEdit:focus, QComboBox:focus, QPlainTextEdit:focus, QTextEdit:focus {
            border: 1px solid #40739e;
            background-color: white;
        }
        QProgressBar {
            border: 1px solid #d0d7de;
            border-radius: 6px;
            text-align: center;
            background-color: #ecf0f1;
            height: 18px;
            font-size: 9pt;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #00a8ff, stop:1 #4cd137);
            border-radius: 6px;
        }
        QPlainTextEdit, QTextEdit {
            background-color: #fafbfc;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 9pt;
        }
        QTabWidget::pane {
            border: 1px solid #d0d7de;
            border-radius: 6px;
            background-color: white;
        }
        QTabBar::tab {
            background-color: #e9ecef;
            border: 1px solid #d0d7de;
            border-bottom: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            padding: 4px 10px;
            margin-right: 2px;
            font-size: 9pt;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 2px solid #00a8ff;
        }
        QTabBar::tab:hover {
            background-color: #dee2e6;
        }
        QLabel#arrowLabel {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #ffffff, stop:1 #f0f2f5);
            border: 2px solid #d0d7de;
            border-radius: 12px;
        }
        """
        self.setStyleSheet(style)

    def init_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        main_layout = QVBoxLayout(central)
        main_layout.setSpacing(12)
        main_layout.setContentsMargins(20, 20, 20, 20)

        top_control_layout = QHBoxLayout()
        top_control_layout.setSpacing(12)

        # ---- 1.1 TCP连接控制 ----
        conn_group = QGroupBox("无线连接 (Android中转)")
        conn_layout = QHBoxLayout(conn_group)
        conn_layout.setSpacing(8)
        conn_layout.setContentsMargins(10, 12, 10, 10)
        conn_layout.addWidget(QLabel("IP:"))
        self.ip_combo = QComboBox()
        self.ip_combo.setEditable(True)
        self.ip_combo.setMinimumWidth(140)
        self.ip_combo.setPlaceholderText("点击搜索或手动输入")
        conn_layout.addWidget(self.ip_combo)
        self.discover_btn = QPushButton("获取网关")
        self.discover_btn.setObjectName("connectBtn")
        self.discover_btn.clicked.connect(self.discover_devices)
        conn_layout.addWidget(self.discover_btn)
        conn_layout.addWidget(QLabel("数据:"))
        self.data_port_edit = QLineEdit(str(FORWARD_PORT))
        self.data_port_edit.setMaximumWidth(70)
        conn_layout.addWidget(self.data_port_edit)
        conn_layout.addWidget(QLabel("命令:"))
        self.cmd_port_edit = QLineEdit(str(CMD_PORT))
        self.cmd_port_edit.setMaximumWidth(70)
        conn_layout.addWidget(self.cmd_port_edit)
        self.connect_btn = QPushButton("连接")
        self.connect_btn.setObjectName("connectBtn")

        self.connect_btn.clicked.connect(self.toggle_connection)
        conn_layout.addWidget(self.connect_btn)
        self.status_label = QLabel("未连接")
        self.status_label.setStyleSheet("color: #e84118; font-weight: bold;")
        conn_layout.addWidget(self.status_label)
        conn_layout.addStretch()
        top_control_layout.addWidget(conn_group, stretch=2)

        # ---- 1.2 EEG模式 ----
        mode_group = QGroupBox("EEG 模式")
        mode_layout = QHBoxLayout(mode_group)
        mode_layout.setSpacing(8)
        mode_layout.setContentsMargins(10, 12, 10, 10)
        mode_layout.addWidget(QLabel("模式:"))
        self.mode_combo = QComboBox()
        self.mode_combo.addItem("COLLECT", 1)
        self.mode_combo.addItem("INFER", 2)
        self.mode_combo.addItem("CSP", 3)
        mode_layout.addWidget(self.mode_combo)
        self.apply_mode_btn = QPushButton("应用")
        self.apply_mode_btn.setObjectName("applyModeBtn")

        self.apply_mode_btn.clicked.connect(self.apply_mode)
        self.apply_mode_btn.setEnabled(False)
        mode_layout.addWidget(self.apply_mode_btn)
        self.mode_status_label = QLabel("COLLECT")
        self.mode_status_label.setStyleSheet("color: #00a8ff; font-weight: bold;")
        mode_layout.addWidget(self.mode_status_label)
        self.model_status_label = QLabel("模型: AUTO")
        self.model_status_label.setStyleSheet("color: #7f8c8d; font-weight: bold;")
        mode_layout.addWidget(self.model_status_label)
        mode_layout.addStretch()
        top_control_layout.addWidget(mode_group, stretch=1)

        # ---- 1.3 控制面板 ----
        control_group = QGroupBox("控制面板")
        control_layout = QVBoxLayout(control_group)
        control_layout.setSpacing(6)
        control_layout.setContentsMargins(10, 12, 10, 10)

        status_row = QHBoxLayout()
        status_row.setSpacing(10)
        self.mode_label = QLabel("空闲")
        self.mode_label.setFont(QFont("Segoe UI", 9, QFont.Bold))
        self.mode_label.setStyleSheet("color: #7f8c8d;")
        status_row.addWidget(self.mode_label)
        self.trial_info = QLabel("")
        self.trial_info.setFont(QFont("Segoe UI", 9))
        status_row.addStretch()
        control_layout.addLayout(status_row)

        btn_row = QHBoxLayout()
        btn_row.setSpacing(6)
        self.train_btn = QPushButton("训练")
        self.train_btn.setObjectName("trainBtn")

        self.train_btn.clicked.connect(self.start_training)
        self.train_btn.setEnabled(False)
        btn_row.addWidget(self.train_btn)

        self.test_btn = QPushButton("测试")
        self.test_btn.setObjectName("testBtn")

        self.test_btn.clicked.connect(self.start_testing)
        self.test_btn.setEnabled(False)
        btn_row.addWidget(self.test_btn)

        self.stop_btn = QPushButton("停止")
        self.stop_btn.setObjectName("stopBtn")

        self.stop_btn.clicked.connect(lambda: self.stop_all(True))
        self.stop_btn.setEnabled(False)
        btn_row.addWidget(self.stop_btn)

        self.status_btn = QPushButton("状态")
        self.status_btn.setObjectName("statusBtn")

        self.status_btn.clicked.connect(self.send_status)
        self.status_btn.setEnabled(False)
        btn_row.addWidget(self.status_btn)

        self.ipcdiag_btn = QPushButton("IPCDIAG: ON")
        self.ipcdiag_btn.setObjectName("ipcdiagBtn")
        self.ipcdiag_btn.setCheckable(True)
        self.ipcdiag_btn.setChecked(True)
        self.ipcdiag_btn.clicked.connect(self.toggle_ipcdiag)
        self.ipcdiag_btn.setEnabled(False)
        btn_row.addWidget(self.ipcdiag_btn)

        self.posture_btn = QPushButton("姿态数据: ON")
        self.posture_btn.setObjectName("postureBtn")
        self.posture_btn.setCheckable(True)
        self.posture_btn.setChecked(True)
        self.posture_btn.clicked.connect(self.toggle_posture)
        self.posture_btn.setEnabled(False)
        btn_row.addWidget(self.posture_btn)

        self.cmd_edit = QLineEdit()
        self.cmd_edit.setPlaceholderText("命令...")
        self.cmd_edit.returnPressed.connect(self.send_manual_cmd)
        btn_row.addWidget(self.cmd_edit)

        control_layout.addLayout(btn_row)
        top_control_layout.addWidget(control_group, stretch=2)
        main_layout.addLayout(top_control_layout)

        # ---- 2. 中央反馈区 ----
        arrow_group = QGroupBox("状态反馈")
        arrow_layout = QVBoxLayout(arrow_group)
        arrow_layout.setContentsMargins(15, 15, 15, 15)
        self.arrow_label = QLabel()
        self.arrow_label.setObjectName("arrowLabel")
        self.arrow_label.setAlignment(Qt.AlignCenter)
        self.arrow_label.setMinimumHeight(160)
        self.arrow_label.setFont(QFont("Segoe UI", 48, QFont.Bold))
        self.arrow_label.setText("空闲")
        self.arrow_label.setStyleSheet("color: #2f3640;")
        arrow_layout.addWidget(self.arrow_label)
        main_layout.addWidget(arrow_group, stretch=1)

        # ---- 3. 进度与状态区 ----
        progress_group = QGroupBox("进度")
        progress_layout = QVBoxLayout(progress_group)
        progress_layout.setSpacing(6)
        progress_layout.setContentsMargins(15, 12, 15, 12)
        self.progress = QProgressBar()
        self.progress.setRange(0, 100)
        self.progress.setValue(0)
        progress_layout.addWidget(self.progress)
        self.stat_label = QLabel("特征: 0 行 | 测试结果: 0")
        self.stat_label.setStyleSheet("color: #2f3640; font-weight: bold; font-size: 9pt;")
        progress_layout.addWidget(self.stat_label)
        self.posture_label = QLabel("姿态角: 等待数据")
        self.posture_label.setToolTip(
            "三轴角来自单片机POSTURE_ANGLE消息，单位为度；偏航角是无磁力计约束的陀螺仪相对积分角，会随时间漂移。"
        )
        self.posture_label.setStyleSheet("color: #00cec9; font-weight: bold; font-size: 9pt;")
        progress_layout.addWidget(self.posture_label)
        main_layout.addWidget(progress_group)

        # ---- 4. 底部日志区 ----
        log_tabs = QTabWidget()
        log_tabs.setDocumentMode(True)
        log_tabs.setTabPosition(QTabWidget.South)

        raw_tab = QWidget()
        raw_layout = QVBoxLayout(raw_tab)
        raw_layout.setContentsMargins(10, 10, 10, 10)
        self.raw_text = QPlainTextEdit()
        self.raw_text.setReadOnly(True)
        self.raw_text.setFont(QFont("Consolas", 9))
        self.raw_text.setMaximumBlockCount(500)
        raw_layout.addWidget(self.raw_text)
        clear_raw_btn = QPushButton("清空原始数据")
        clear_raw_btn.setStyleSheet("background-color: #7f8c8d; padding: 4px 12px; font-size: 9pt;")

        clear_raw_btn.clicked.connect(lambda: self.raw_text.clear())
        raw_layout.addWidget(clear_raw_btn, alignment=Qt.AlignRight)
        log_tabs.addTab(raw_tab, "原始数据")

        log_tab = QWidget()
        log_layout = QVBoxLayout(log_tab)
        log_layout.setContentsMargins(10, 10, 10, 10)
        self.result_text = QTextEdit()
        self.result_text.setReadOnly(True)
        self.result_text.setFont(QFont("Consolas", 9))
        log_layout.addWidget(self.result_text)
        log_tabs.addTab(log_tab, "解析日志")

        main_layout.addWidget(log_tabs, stretch=2)

        self.update_central_status("空闲", "#7f8c8d")

    # ======================= 中央状态更新 =======================
    def update_central_status(self, text, color="#2f3640", is_arrow=False):
        if is_arrow:
            border_color = "#0984e3" if "左" in text else "#e84118"
            self.arrow_label.setStyleSheet(
                f"color: {color}; background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                f"stop:0 #dfe6e9, stop:1 #b2bec3); font-size: 56pt; font-weight: bold; "
                f"border-radius: 12px; border: 3px solid {border_color};"
            )
        else:
            self.arrow_label.setStyleSheet(
                f"color: {color}; background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                f"stop:0 #f0f2f5, stop:1 #dcdde1); font-size: 40pt; font-weight: bold; "
                f"border-radius: 12px; border: 2px solid #d0d7de;"
            )
        self.arrow_label.setText(text)

    # ======================= 连接管理 =======================
    def discover_devices(self):
        ip = get_default_gateway()
        self.ip_combo.setCurrentText(ip)
        self.status_label.setText(f"网关: {ip}")
        self.status_label.setStyleSheet("color: #4cd137; font-weight: bold;")
        self.append_log(f"[系统] 自动获取网关IP: {ip}", "green")

    def toggle_connection(self):
        if self.tcp_thread and self.tcp_thread.isRunning():
            self.tcp_thread.stop()
            self.tcp_thread = None
            self.connect_btn.setText("连接")

            self.connect_btn.setStyleSheet("background-color: #4cd137;")
            self.status_label.setText("未连接")
            self.status_label.setStyleSheet("color: #e84118; font-weight: bold;")
            self.train_btn.setEnabled(False)
            self.test_btn.setEnabled(False)
            self.stop_btn.setEnabled(False)
            self.status_btn.setEnabled(False)
            self.apply_mode_btn.setEnabled(False)
            self.ipcdiag_btn.setEnabled(False)
            self.posture_btn.setEnabled(False)
            self.model_status_label.setText("模型: 未知")
            self.model_status_label.setStyleSheet("color: #7f8c8d; font-weight: bold;")
            self.append_log("[系统] TCP已断开", "gray")
            self.update_central_status("空闲 (TCP断开)", "#7f8c8d")
            return

        ip = self.ip_combo.currentText().strip()
        if not ip:
            ip = get_default_gateway()
            self.ip_combo.setCurrentText(ip)
            self.append_log(f"[系统] 自动获取网关IP: {ip}", "green")
        try:
            data_port = int(self.data_port_edit.text().strip())
            cmd_port = int(self.cmd_port_edit.text().strip())
        except ValueError:
            QMessageBox.warning(self, "提示", "端口号必须为整数")
            return

        self.tcp_thread = TcpThread(ip, data_port, cmd_port)
        self.tcp_thread.running = True
        self.tcp_thread.data_received.connect(self.parse_serial_line)
        self.tcp_thread.start()
        self.connect_btn.setText("断开")

        self.connect_btn.setStyleSheet("background-color: #e84118;")
        self.status_label.setText("连接中...")
        self.status_label.setStyleSheet("color: #e1b12c; font-weight: bold;")
        self.append_log(f"[系统] 正在连接 {ip}:{data_port}/{cmd_port} ...", "cyan")

    def send_manual_cmd(self):
        cmd = self.cmd_edit.text().strip()
        if cmd and self.tcp_thread:
            self.tcp_thread.send_command(cmd)
            self.append_log(f"[手动发送] {cmd}", "magenta")
            self.cmd_edit.clear()

    def send_status(self):
        if self.tcp_thread:
            self.tcp_thread.send_command("STATUS")

    def toggle_ipcdiag(self):
        if not self.tcp_thread:
            return
        if self.ipcdiag_btn.isChecked():
            self.tcp_thread.send_command("IPCDIAG,ON")
            self.ipcdiag_btn.setText("IPCDIAG: ON")
            self.append_log("[系统] IPCDIAG 已开启", "green")
        else:
            self.tcp_thread.send_command("IPCDIAG,OFF")
            self.ipcdiag_btn.setText("IPCDIAG: OFF")
            self.append_log("[系统] IPCDIAG 已关闭", "orange")

    def toggle_posture(self):
        if not self.tcp_thread:
            return
        if self.posture_btn.isChecked():
            self.tcp_thread.send_command("POSTURE,ON")
            self.posture_btn.setText("姿态数据: ON")
            self.append_log("[系统] 姿态数据流已开启", "green")
        else:
            self.tcp_thread.send_command("POSTURE,OFF")
            self.posture_btn.setText("姿态数据: OFF")
            self.append_log("[系统] 姿态数据流已关闭（事件仍正常输出）", "orange")

    def apply_mode(self):
        if not self.tcp_thread:
            QMessageBox.warning(self, "提示", "请先连接")
            return
        mode = self.mode_combo.currentData()
        self.tcp_thread.send_command(f"MODE,SET,{mode}")
        self.append_log(f"[系统] 发送模式切换: {mode}", "cyan")

    def update_model_status(self, model, actual=False):
        model = str(model).upper()
        colors = {"AUTO": "#7f8c8d", "FFT24": "#0984e3", "CSP": "#00b894"}
        prefix = "实际" if actual else "模型"
        self.model_status_label.setText(f"{prefix}: {model}")
        self.model_status_label.setStyleSheet(
            f"color: {colors.get(model, '#7f8c8d')}; font-weight: bold;"
        )

    # ======================= 日志 =======================
    def append_log(self, text, color=None):
        if color:
            self.result_text.append(f'<span style="color:{color};">{text}</span>')
        else:
            self.result_text.append(text)
        self.result_text.moveCursor(QTextCursor.End)

    def append_raw(self, line):
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.raw_text.appendPlainText(f"[{timestamp}] {line}")
        cursor = self.raw_text.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.raw_text.setTextCursor(cursor)

    # ======================= 数据解析 =======================
    def _send_ack(self, line):
        m = re.search(r'seq=(\d+)', line)
        if m:
            if self.tcp_thread:
                self.tcp_thread.send_command(f"ACK,{m.group(1)}")

    def _is_dup_seq(self, line):
        m = re.search(r'seq=(\d+)', line)
        if m:
            seq = int(m.group(1))
            if seq == self._last_seq:
                return True
            self._last_seq = seq
        return False

    def parse_serial_line(self, line):
        if line == "TCP_CONNECTED":
            self.status_label.setText("已连接")
            self.status_label.setStyleSheet("color: #4cd137; font-weight: bold;")
            self.train_btn.setEnabled(True)
            self.test_btn.setEnabled(True)
            self.status_btn.setEnabled(True)
            self.apply_mode_btn.setEnabled(True)
            self.ipcdiag_btn.setEnabled(True)
            self.posture_btn.setEnabled(True)
            self.append_log("[系统] TCP已连接", "green")
            self.tcp_thread.send_command("STATUS")
            self.tcp_thread.send_command("MODEL,GET")
            self.update_central_status("空闲", "#7f8c8d")
            return

        if line.startswith("TCP_ERROR:"):
            self.append_log(f"[TCP错误] {line[10:]}", "red")
            if self.tcp_thread:
                self.tcp_thread.stop()
            self.tcp_thread = None
            self.connect_btn.setText("连接")
            self.connect_btn.setStyleSheet("background-color: #4cd137;")
            self.status_label.setText("连接失败")
            self.status_label.setStyleSheet("color: #e84118; font-weight: bold;")
            self.train_btn.setEnabled(False)
            self.test_btn.setEnabled(False)
            self.stop_btn.setEnabled(False)
            self.status_btn.setEnabled(False)
            self.apply_mode_btn.setEnabled(False)
            self.ipcdiag_btn.setEnabled(False)
            self.posture_btn.setEnabled(False)
            self.update_central_status("连接失败", "#e84118")
            return

        self.append_raw(line)

        if re.search(r'seq=\d+', line):
            self._send_ack(line)
            if self._is_dup_seq(line):
                return

        match = re.match(r'TASK,(\w+),start', line)
        if match:
            side = match.group(1)
            if side == 'LEFT':
                self.show_arrow_left()
                self.append_log(">>> 左手任务开始 <<<", "blue")
            else:
                self.show_arrow_right()
                self.append_log(">>> 右手任务开始 <<<", "red")
            if self.is_training:
                total = len(self.trial_sequence)
                cur = self.current_trial_index + 1
                self.trial_info.setText(f"试次 {cur}/{total} ({side})")
            return

        if line.startswith('TASK,DONE'):
            self.append_log("--- 试次完成 ---", "green")
            self.on_trial_done()
            return

        if line.startswith('TASK,STOPPED'):
            self.append_log("[系统] 已停止", "orange")
            self.stop_all(False)
            return

        match = re.match(r'DIRCSV,(\d+),(.+)$', line)
        if match:
            label = int(match.group(1))
            feat_str = match.group(2)
            try:
                features = [int(x) for x in feat_str.split(',') if x.strip()]
                if len(features) == 24:
                    self.train_data.append((label, features))
                    self.dir_row_trial_ids.append(
                        self.current_trial_index if self.is_training else None
                    )
                    self.received_feature_count += 1
                    self.stat_label.setText(
                        f"特征: {self.received_feature_count} 行 | 测试结果: {len(self.test_results)}"
                    )
                    if self.is_training:
                        total = len(self.trial_sequence)
                        done = len(self.train_data) // 20
                        progress = min(100, int(done / total * 100))
                        self.progress.setValue(progress)
            except:
                pass
            return

        if line.startswith('CSP_BEGIN,'):
            parts = line.split(',')
            try:
                self.csp_label = int(parts[1].split('=')[1])
                self.csp_win_id = int(parts[2].split('=')[1])
                self.csp_window_buffer = {}
                self.csp_receiving = True
                self.append_log(f"[CSP] 窗口 {self.csp_win_id} 开始 (label={self.csp_label})", "cyan")
            except:
                pass
            return

        if line.startswith('CSP,'):
            if self.csp_receiving:
                parts = line.split(',')
                try:
                    idx = int(parts[2])
                    chs = [int(parts[3]), int(parts[4]), int(parts[5]), int(parts[6])]
                    if 0 <= idx < 256:
                        self.csp_window_buffer[idx] = chs
                except:
                    pass
            return

        if line.startswith('CSP_END,'):
            if self.csp_receiving:
                self.csp_receiving = False
                end_match = re.match(r'CSP_END,label=(\d+),win=(\d+)', line)
                end_ok = (
                    end_match is not None
                    and int(end_match.group(1)) == self.csp_label
                    and int(end_match.group(2)) == self.csp_win_id
                )
                complete = (
                    end_ok
                    and len(self.csp_window_buffer) == 256
                    and all(idx in self.csp_window_buffer for idx in range(256))
                )
                if not complete:
                    self.append_log(
                        f"[CSP错误] 窗口 {self.csp_win_id} 不完整或结束标识不匹配，已丢弃 "
                        f"({len(self.csp_window_buffer)}/256)",
                        "red",
                    )
                    return

                ordered_rows = [(idx, self.csp_window_buffer[idx]) for idx in range(256)]
                self.csp_data.append((self.csp_label, self.csp_win_id, ordered_rows))
                self.csp_window_trial_map[(self.csp_label, self.csp_win_id)] = self.current_trial_index + 1
                self.received_feature_count += 1
                self.stat_label.setText(
                    f"特征: {self.received_feature_count} 行 | 测试结果: {len(self.test_results)}"
                )
                if self.is_training:
                    total = len(self.trial_sequence)
                    done = len(self.csp_data) // CSP_WINDOWS_PER_TRIAL
                    progress = min(100, int(done / total * 100))
                    self.progress.setValue(progress)
                self.append_log(f"[CSP] 窗口 {self.csp_win_id} 完成", "cyan")
            return

        if 'INTENT=' in line and 'S_LEFT=' in line and 'S_RIGHT=' in line:
            intent_match = re.search(r'INTENT=(\w+)', line)
            s_left_match = re.search(r'S_LEFT=(\d+)', line)
            s_right_match = re.search(r'S_RIGHT=(\d+)', line)
            if intent_match and s_left_match and s_right_match:
                intent = intent_match.group(1)
                sl = int(s_left_match.group(1))
                sr = int(s_right_match.group(1))
                self.test_results.append((intent, sl, sr))
                self.stat_label.setText(
                    f"特征: {self.received_feature_count} 行 | 测试结果: {len(self.test_results)}"
                )
                if intent == 'LEFT':
                    self.show_arrow_left()
                    color = "blue"
                elif intent == 'RIGHT':
                    self.show_arrow_right()
                    color = "red"
                else:
                    color = "black"
                conf = abs(sl - sr)
                model_match = re.search(r'MODEL=(\w+)', line)
                actual_model = model_match.group(1).upper() if model_match else ""
                if actual_model:
                    self.update_model_status(actual_model, actual=True)
                    if self.infer_model_select == "CSP" and actual_model == "FFT24":
                        self.append_log("[模型] CSP权重尚未生效，本次实际回退FFT24", "orange")
                model_text = f", 模型:{actual_model}" if actual_model else ""
                self.append_log(
                    f"[TEST] {intent} (左:{sl}, 右:{sr}, 置信度:{conf}{model_text})",
                    color,
                )
                return

        if line == 'READY_TRAIN':
            self.append_log("[系统] 训练模式就绪", "green")
            self.mode_label.setText("训练中")
            self.mode_label.setStyleSheet("color: #00a8ff; font-weight: bold;")
            self.update_central_status("训练中", "#00a8ff")
            self.waiting_for_ready = False
            if self.is_training:
                self.send_next_trial()
            return

        if line == 'READY_TEST':
            self.append_log("[系统] 测试模式就绪", "green")
            self.mode_label.setText("测试中")
            self.mode_label.setStyleSheet("color: #e1b12c; font-weight: bold;")
            self.update_central_status("测试中", "#e1b12c")
            self.waiting_for_ready = False
            return

        if line.startswith('MODE_SET_OK,'):
            mode = int(line.split(',')[1])
            mode_names = {1: "COLLECT", 2: "INFER", 3: "CSP"}
            name = mode_names.get(mode, "UNKNOWN")
            self.mode_status_label.setText(name)
            color = "blue" if mode == 1 else "red" if mode == 2 else "green"
            self.mode_status_label.setStyleSheet(f"color: {color}; font-weight: bold;")
            self.append_log(f"[系统] 模式已切换至: {name}", "cyan")
            idx = self.mode_combo.findData(mode)
            if idx >= 0:
                self.mode_combo.setCurrentIndex(idx)

            work_command = None
            if self.pending_train:
                self.pending_train = False
                work_command = "MODE,TRAIN"
            if self.pending_test:
                self.pending_test = False
                work_command = "MODE,TEST"

            bound_model = {1: "FFT24", 3: "CSP"}.get(mode)
            if bound_model:
                self.pending_work_command = work_command
                self.tcp_thread.send_command(f"MODEL,SET,{bound_model}")
                self.append_log(
                    f"[模型] {name} 已绑定 {bound_model}，等待下位机确认",
                    "magenta",
                )
            elif work_command:
                self.pending_work_command = None
                self.tcp_thread.send_command(work_command)
                self.append_log(f"[系统] 发送 {work_command}", "magenta")
            return

        if line.startswith('MODEL_SET_OK,'):
            model = line.split(',', 1)[1].strip().upper()
            if model in ("AUTO", "FFT24", "CSP"):
                self.infer_model_select = model
                self.update_model_status(model)
                self.append_log(f"[模型] 已切换至 {model}", "green")
            if self.pending_work_command:
                command = self.pending_work_command
                self.pending_work_command = None
                self.tcp_thread.send_command(command)
                self.append_log(f"[系统] 发送 {command}", "magenta")
            return

        if line.startswith('MODEL,'):
            model = line.split(',', 1)[1].strip().upper()
            if model in ("AUTO", "FFT24", "CSP"):
                self.infer_model_select = model
                self.update_model_status(model)
                self.append_log(f"[模型] 当前选择: {model}", "cyan")
            return

        if line.startswith('ERROR'):
            self.append_log(f"[错误] {line}", "red")
            QMessageBox.warning(self, "下位机错误", line)
            return

        if line.startswith('STATUS,'):
            self.append_log(f"[状态] {line}", "cyan")
            parts = line.split(',')
            for part in parts:
                if part.startswith('app_mode='):
                    mode = int(part.split('=')[1])
                    mode_names = {1: "COLLECT", 2: "INFER", 3: "CSP"}
                    name = mode_names.get(mode, "UNKNOWN")
                    self.mode_status_label.setText(name)
                    color = "blue" if mode == 1 else "red" if mode == 2 else "green"
                    self.mode_status_label.setStyleSheet(f"color: {color}; font-weight: bold;")
                    idx = self.mode_combo.findData(mode)
                    if idx >= 0:
                        self.mode_combo.setCurrentIndex(idx)
                elif part.startswith('mode='):
                    work_mode = int(part.split('=')[1])
                    work_names = {0: "空闲", 1: "训练中", 2: "测试中"}
                    work_colors = {0: "#7f8c8d", 1: "#00a8ff", 2: "#e1b12c"}
                    if work_mode in work_names:
                        self.mode_label.setText(work_names[work_mode])
                        self.mode_label.setStyleSheet(
                            f"color: {work_colors[work_mode]}; font-weight: bold;"
                        )
            return

        if line == 'IPCDIAG,ON':
            self.ipcdiag_btn.setChecked(True)
            self.ipcdiag_btn.setText("IPCDIAG: ON")
            self.append_log("[系统] IPCDIAG 已开启", "green")
            return
        if line == 'IPCDIAG,OFF':
            self.ipcdiag_btn.setChecked(False)
            self.ipcdiag_btn.setText("IPCDIAG: OFF")
            self.append_log("[系统] IPCDIAG 已关闭", "orange")
            return

        if line == 'POSTURE,ON':
            self.posture_btn.setChecked(True)
            self.posture_btn.setText("姿态数据: ON")
            self.append_log("[系统] 姿态数据流已开启", "green")
            return
        if line == 'POSTURE,OFF':
            self.posture_btn.setChecked(False)
            self.posture_btn.setText("姿态数据: OFF")
            self.append_log("[系统] 姿态数据流已关闭（事件仍正常输出）", "orange")
            return

        match = re.match(r'POSTURE_STATE,(?:seq=\d+,)?(\w+),turns=(\d+)(?:,gx=(-?\d+),gy=(-?\d+),gz=(-?\d+),conf=(-?\d+),stable=(\d+))?', line)
        if match:
            self.posture_name = match.group(1)
            self.turn_count = int(match.group(2))
            if match.group(3):
                self.posture_gx = int(match.group(3))
                self.posture_gy = int(match.group(4))
                self.posture_gz = int(match.group(5))
                self.posture_conf = int(match.group(6))
                self.posture_stable_ms = int(match.group(7))
                self.posture_metrics_valid = True
            else:
                self.posture_metrics_valid = False
            self.update_posture_display()
            return

        match = re.match(
            r'POSTURE_ANGLE,roll=(-?\d+(?:\.\d+)?),pitch=(-?\d+(?:\.\d+)?),yaw=(-?\d+(?:\.\d+)?)',
            line,
        )
        if match:
            self.posture_roll_deg = float(match.group(1))
            self.posture_pitch_deg = float(match.group(2))
            self.posture_yaw_deg = float(match.group(3))
            self.update_posture_display()
            return

        match = re.match(
            r'POSTURE_ANGLE,roll10=(-?\d+),pitch10=(-?\d+),yaw10=(-?\d+)',
            line,
        )
        if match:
            self.posture_roll_deg = int(match.group(1)) / 10.0
            self.posture_pitch_deg = int(match.group(2)) / 10.0
            self.posture_yaw_deg = int(match.group(3)) / 10.0
            self.update_posture_display()
            return

        match = re.match(
            r'POSTURE,(\w+),gx=(-?\d+),gy=(-?\d+),gz=(-?\d+),conf=(-?\d+),stable=(\d+)',
            line,
        )
        if match:
            self.posture_name = match.group(1)
            self.posture_gx = int(match.group(2))
            self.posture_gy = int(match.group(3))
            self.posture_gz = int(match.group(4))
            self.posture_conf = int(match.group(5))
            self.posture_stable_ms = int(match.group(6))
            self.posture_metrics_valid = True
            self.update_posture_display()
            return

        match = re.match(r'PSTATUS,(\w+),conf=(-?\d+),stable=(\d+),turns=(\d+)', line)
        if match:
            self.posture_name = match.group(1)
            self.posture_conf = int(match.group(2))
            self.posture_stable_ms = int(match.group(3))
            self.turn_count = int(match.group(4))
            self.posture_metrics_valid = True
            self.update_posture_display()
            return

        match = re.match(r'TURN_EVENT,(?:seq=\d+,)?count=(\d+),from=(\w+),to=(\w+)(?:,tick=\d+)?', line)
        if match:
            self.turn_count = int(match.group(1))
            turn_from = match.group(2)
            turn_to = match.group(3)
            self.posture_name = turn_to
            self.append_log(f"[翻身] 第{self.turn_count}次: {turn_from} -> {turn_to}", "#00cec9")
            self.update_posture_display()
            return

        match = re.match(r'FALL_EVENT,(?:seq=\d+,)?gyro=(-?\d+),acc=(-?\d+),posture=(\w+)(?:,tick=\d+)?', line)
        if match:
            gyro = int(match.group(1))
            acc = int(match.group(2))
            fall_posture = match.group(3)
            self.fall_detected = True
            self.posture_name = fall_posture
            self.append_log(f"[!!坠床!!] 角速度={gyro/100.0:.2f}dps 加速度={acc/100.0:.2f}g", "red")
            self.update_posture_display()
            QTimer.singleShot(10000, self.clear_fall_alert)
            return

        match = re.match(r'NO_TURN_ALERT,(?:seq=\d+,)?duration_min=(\d+)', line)
        if match:
            dur = int(match.group(1))
            self.append_log(f"[未翻身报警] 已{dur}分钟未翻身!", "red")
            return

    # ======================= 箭头显示 =======================
    def show_arrow_left(self):
        self.update_central_status("← 左手", "#0984e3", is_arrow=True)

    def show_arrow_right(self):
        self.update_central_status("→ 右手", "#e84118", is_arrow=True)

    def show_arrow_ready(self):
        if self.is_training:
            self.update_central_status("训练中", "#00a8ff")
        elif self.is_testing:
            self.update_central_status("测试中", "#e1b12c")
        else:
            self.update_central_status("空闲", "#7f8c8d")

    # ======================= 姿态显示 =======================
    @staticmethod
    def gravity_to_posture_angles(gx_mg, gy_mg, gz_mg):
        gx = float(gx_mg)
        gy = float(gy_mg)
        gz = float(gz_mg)
        norm = math.sqrt(gx * gx + gy * gy + gz * gz)
        if norm < 1.0e-6:
            return None, None, None

        roll_deg = math.degrees(math.atan2(gy, gz))
        pitch_deg = math.degrees(math.atan2(-gx, math.sqrt(gy * gy + gz * gz)))
        yaw_deg = None
        return roll_deg, pitch_deg, yaw_deg

    def posture_angle_text(self):
        if (
            self.posture_roll_deg is not None
            and self.posture_pitch_deg is not None
            and self.posture_yaw_deg is not None
        ):
            return (
                f"横滚={self.posture_roll_deg:+.1f}°  "
                f"俯仰={self.posture_pitch_deg:+.1f}°  "
                f"偏航={self.posture_yaw_deg:+.1f}°"
            )

        roll_deg, pitch_deg, yaw_deg = self.gravity_to_posture_angles(
            self.posture_gx,
            self.posture_gy,
            self.posture_gz,
        )
        if roll_deg is None or pitch_deg is None:
            return "横滚=--  俯仰=--  偏航=不可观测"
        yaw_text = f"{yaw_deg:+.1f}°" if yaw_deg is not None else "不可观测"
        return (
            f"横滚={roll_deg:+.1f}°  俯仰={pitch_deg:+.1f}°  "
            f"偏航={yaw_text}"
        )

    def update_posture_display(self):
        posture_cn = {
            "SUPINE": "仰躺", "PRONE": "趴着",
            "LEFT": "左侧卧", "RIGHT": "右侧卧",
            "SITTING": "坐起", "UNKNOWN": "未知"
        }
        name = posture_cn.get(self.posture_name, self.posture_name)
        conf_pct = self.posture_conf / 100.0
        stable_s = self.posture_stable_ms / 1000.0
        angle_text = self.posture_angle_text()

        if self.fall_detected:
            self.posture_label.setText(
                f"!! 坠床报警 !!  姿态: {name}  {angle_text}  翻身: {self.turn_count}次"
            )
            self.posture_label.setStyleSheet("color: #e84118; font-weight: bold; font-size: 11pt; background-color: #ffeaa7;")
            return

        if self.posture_metrics_valid:
            posture_text = f"姿态: {name} ({conf_pct:.0f}%)  稳定: {stable_s:.1f}s"
        else:
            posture_text = f"姿态: {name}"
        self.posture_label.setText(
            f"{posture_text}  姿态角: {angle_text}  翻身: {self.turn_count}次"
        )
        self.posture_label.setStyleSheet("color: #00cec9; font-weight: bold; font-size: 9pt;")

    def clear_fall_alert(self):
        self.fall_detected = False
        self.update_posture_display()

    # ======================= 训练流程 =======================
    @staticmethod
    def build_balanced_trial_sequence(per_class, max_same_in_a_row=2):
        base = ['LEFT'] * per_class + ['RIGHT'] * per_class
        for _ in range(500):
            random.shuffle(base)
            run = 1
            valid = True
            for i in range(1, len(base)):
                run = run + 1 if base[i] == base[i - 1] else 1
                if run > max_same_in_a_row:
                    valid = False
                    break
            if valid:
                return list(base)

        first = random.choice(['LEFT', 'RIGHT'])
        second = 'RIGHT' if first == 'LEFT' else 'LEFT'
        return [first if i % 2 == 0 else second for i in range(per_class * 2)]

    def start_training(self):
        if not self.tcp_thread:
            return

        mode = self.mode_combo.currentData()
        if mode not in (1, 3):
            self.append_log("[警告] 训练模式仅支持 COLLECT 或 CSP，自动切换至 COLLECT", "orange")
            mode = 1
            self.mode_combo.setCurrentIndex(0)

        if mode == 3:
            self.trial_sequence = self.build_balanced_trial_sequence(CSP_TRIALS_PER_CLASS)
            train_desc = f"CSP训练（{len(self.trial_sequence)}组，左右各{CSP_TRIALS_PER_CLASS}组，平衡随机）"
        else:
            self.trial_sequence = ['LEFT', 'RIGHT', 'LEFT', 'RIGHT']
            train_desc = "24维训练（4组，左右左右）"

        self.current_trial_index = 0
        self.train_data.clear()
        self.dir_row_trial_ids.clear()
        self.csp_data.clear()
        self.csp_window_trial_map.clear()
        self.received_feature_count = 0
        self.is_training = True
        self.is_testing = False
        self.waiting_for_ready = True
        self.pending_train = True
        self.pending_test = False
        self.progress.setValue(0)
        self.stat_label.setText("特征: 0 行 | 测试结果: 0")
        self.trial_info.setText("")

        self.append_log("="*60, "blue")
        self.append_log(f"=== 开始{train_desc} ===", "blue")
        self.append_log(f"顺序: {' -> '.join(self.trial_sequence)}", "blue")
        self.append_log("="*60, "blue")

        self.train_btn.setEnabled(False)
        self.test_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)

        self.update_central_status("训练启动中...", "#00a8ff")
        self.tcp_thread.send_command(f"MODE,SET,{mode}")

    def send_next_trial(self):
        if self.current_trial_index < len(self.trial_sequence):
            side = self.trial_sequence[self.current_trial_index]
            self.tcp_thread.send_command(f"TRIAL,{side}")
            self.append_log(
                f">>> 发送 {side} 试次 ({self.current_trial_index+1}/{len(self.trial_sequence)})",
                "magenta"
            )
        else:
            self.append_log("所有试次已发送，等待数据...", "green")

    def on_trial_done(self):
        if self.is_training:
            self.current_trial_index += 1
            if self.current_trial_index < len(self.trial_sequence):
                self.send_next_trial()
            else:
                self.append_log("="*60, "green")
                self.append_log(f"=== 训练完成！共接收 {self.received_feature_count} 个特征 ===", "green")
                self.append_log("="*60, "green")
                self.stop_all(False)
                self.save_training_data()
                self.update_central_status("训练完成", "#4cd137")

    def save_training_data(self):
        if self.train_data:
            filename = f"train_DIRCSV_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
            target_dir = Path(__file__).resolve().parent / "direction_24d_pycharm" / "data"
            output_path = target_dir / filename
            try:
                target_dir.mkdir(parents=True, exist_ok=True)
                with open(output_path, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['label'] + [f'feat{i}' for i in range(24)])
                    for label, feats in self.train_data:
                        writer.writerow([label] + feats)

                meta_path = output_path.with_name(output_path.stem + '_meta.json')
                meta = {
                    'csv_file': output_path.name,
                    'trial_sequence': self.trial_sequence,
                    'rows': [
                        {
                            'row_index': index,
                            'label': self.train_data[index][0],
                            'trial_id': trial_id,
                        }
                        for index, trial_id in enumerate(self.dir_row_trial_ids)
                    ],
                }
                meta_path.write_text(
                    json.dumps(meta, ensure_ascii=False, indent=2),
                    encoding='utf-8',
                )
                self.append_log(f"[保存] DIRCSV 数据已保存至 {output_path.resolve()}", "green")
                self.append_log(f"[保存] 24维试次元数据已保存至 {meta_path.resolve()}", "green")
            except Exception as e:
                self.append_log(f"[错误] 保存失败: {e}", "red")
        elif self.csp_data:
            filename = f"train_CSP_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
            target_dir = Path(__file__).resolve().parent / "python_csp" / "data"
            output_path = target_dir / filename
            try:
                target_dir.mkdir(parents=True, exist_ok=True)
                with open(output_path, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['label', 'win_id', 'sample_idx', 'ch0', 'ch1', 'ch2', 'ch3'])
                    for label, win_id, samples in self.csp_data:
                        for idx, chs in samples:
                            writer.writerow([label, win_id, idx] + chs)

                meta_path = output_path.with_name(output_path.stem + '_meta.json')
                meta = {
                    'csv_file': output_path.name,
                    'trial_sequence': self.trial_sequence,
                    'windows_per_trial_expected': CSP_WINDOWS_PER_TRIAL,
                    'windows': [
                        {
                            'label': label,
                            'win_id': win_id,
                            'trial_id': self.csp_window_trial_map.get((label, win_id)),
                        }
                        for label, win_id, _ in self.csp_data
                    ],
                }
                meta_path.write_text(json.dumps(meta, ensure_ascii=False, indent=2), encoding='utf-8')
                self.append_log(f"[保存] CSP 数据已保存至 {output_path.resolve()}", "green")
                self.append_log(f"[保存] 试次元数据已保存至 {meta_path.resolve()}", "green")
            except Exception as e:
                self.append_log(f"[错误] 保存失败: {e}", "red")
        else:
            self.append_log("[警告] 无数据可保存", "orange")

    # ======================= 测试流程 =======================
    def start_testing(self):
        if not self.tcp_thread:
            return

        mode = 2
        self.mode_combo.setCurrentIndex(1)
        self.pending_test = True
        self.pending_train = False
        self.is_testing = True
        self.is_training = False
        self.test_results.clear()
        self.stat_label.setText(f"特征: {self.received_feature_count} 行 | 测试结果: 0")
        self.show_arrow_ready()

        self.append_log("="*60, "red")
        self.append_log("=== 开始测试（推理模式） ===", "red")
        self.append_log("等待模式切换...", "red")
        self.append_log("="*60, "red")

        self.train_btn.setEnabled(False)
        self.test_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)

        self.update_central_status("测试启动中...", "#e1b12c")
        self.tcp_thread.send_command(f"MODE,SET,{mode}")

    # ======================= 停止 =======================
    def stop_all(self, send_cmd=True):
        if send_cmd and self.tcp_thread:
            self.tcp_thread.send_command("STOP")
        self.is_training = False
        self.is_testing = False
        self.waiting_for_ready = False
        self.pending_train = False
        self.pending_test = False
        self.pending_work_command = None
        self.train_btn.setEnabled(True)
        self.test_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.progress.setValue(0)
        self.mode_label.setText("空闲")
        self.mode_label.setStyleSheet("color: #7f8c8d; font-weight: bold;")
        self.trial_info.setText("")
        self.append_log("[系统] 已停止所有活动", "orange")
        self.update_central_status("空闲", "#7f8c8d")

    def closeEvent(self, event):
        if self.tcp_thread:
            self.tcp_thread.stop()
        event.accept()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec_())