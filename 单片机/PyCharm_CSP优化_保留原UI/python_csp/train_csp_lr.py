#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
四通道正则化 CSP + 线性分类器自动比较 + MCU 权重导出。

保留原有能力：
1. 读取上位机表格 CSV 或 CSP_BEGIN/CSP/CSP_END 串口日志；
2. 通道顺序固定为 CP3、CP4、C3、C4；
3. 固件标签 LEFT=2、RIGHT=3；
4. 自动生成并覆盖 Common/DualCore_CSP_Model_4ch_Weights.h；
5. MCU 端仍使用原变量名 csp4ch_svm_weight / csp4ch_svm_bias，
   因此不需要修改现有 V5F 推理代码。

本版只优化电脑端训练：
- 优先读取上位机额外保存的 *_meta.json，准确区分独立试次；
- 无元数据时按标签变化或每4个窗口推断试次；
- 正则化 CSP；
- 比较全部4个CSP分量和两端2个CSP分量；
- 自动比较 Linear SVM、正则化 LDA、Logistic Regression；
- 按独立试次验证；有多个文件时额外进行跨文件/跨会话验证；
- 自动选择准确率高且不过度偏向单一类别的模型；
- 统一导出为 MCU 可直接执行的线性 weight + bias。

训练与 MCU 推理保持一致：
    每通道去均值 -> CSP投影 -> 方差 -> log(var/sum(var))
    -> 标准化 -> 线性判别。
"""
from __future__ import annotations

import argparse
import csv
import json
import re
import shutil
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple, Union

import joblib
import numpy as np
from scipy.linalg import eigh
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import balanced_accuracy_score, confusion_matrix
from sklearn.model_selection import StratifiedGroupKFold
from sklearn.preprocessing import StandardScaler
from sklearn.svm import LinearSVC

LEFT_LABEL = 2
RIGHT_LABEL = 3
LABEL_TO_CLASS = {LEFT_LABEL: 0, RIGHT_LABEL: 1}
CLASS_NAMES = {0: "LEFT", 1: "RIGHT"}
EXPECTED_CHANNELS = 4
EXPECTED_SAMPLES = 256
EXPECTED_WINDOWS_PER_TRIAL = 4
EPS = 1.0e-18
RANDOM_STATE = 17

BEGIN_RE = re.compile(r"CSP_BEGIN,.*?label=(\d+),.*?win=(\d+).*", re.IGNORECASE)
ROW_RE = re.compile(r"CSP,(\d+),(\d+),(-?\d+),(-?\d+),(-?\d+),(-?\d+)", re.IGNORECASE)
END_RE = re.compile(r"CSP_END,.*?label=(\d+),.*?win=(\d+).*", re.IGNORECASE)

ParamValue = Union[float, str]


@dataclass
class CSPWindow:
    """一个完整的4×256 CSP训练窗口。"""

    data_uv: np.ndarray
    label: int
    source_file: str
    window_id: int
    trial_hint: Optional[int] = None
    trial_group: str = ""


@dataclass(frozen=True)
class Candidate:
    cov_reg: float
    classifier: str
    classifier_param: ParamValue
    feature_mode: str


@dataclass
class FittedPipeline:
    csp_matrix: np.ndarray
    scaler: StandardScaler
    classifier: object
    feature_indices: np.ndarray
    raw_features: np.ndarray


def _prepare_window(raw_rows: Sequence[Sequence[int]]) -> np.ndarray:
    """将uV×1000整数恢复为uV，并逐通道去均值。"""
    arr = np.asarray(raw_rows, dtype=np.float64).T
    if arr.shape != (EXPECTED_CHANNELS, EXPECTED_SAMPLES):
        raise ValueError(f"窗口形状错误：{arr.shape}，应为(4, 256)")
    arr = arr / 1000.0
    arr -= arr.mean(axis=1, keepdims=True)
    return arr


def _load_trial_metadata(path: Path) -> Dict[Tuple[int, int], int]:
    """读取上位机旁路元数据；主CSP CSV格式保持不变。"""
    meta_path = path.with_name(path.stem + "_meta.json")
    if not meta_path.exists():
        return {}
    try:
        payload = json.loads(meta_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        print(f"[警告] 无法读取试次元数据 {meta_path.name}: {exc}")
        return {}

    result: Dict[Tuple[int, int], int] = {}
    for item in payload.get("windows", []):
        try:
            label = int(item["label"])
            win_id = int(item["win_id"])
            trial_id = int(item["trial_id"])
        except (KeyError, TypeError, ValueError):
            continue
        result[(label, win_id)] = trial_id
    if result:
        print(f"[元数据] {meta_path.name}: 读取 {len(result)} 个窗口的独立试次编号")
    return result


def parse_table_csv(path: Path) -> List[CSPWindow]:
    """读取上位机保存的 label/win_id/sample_idx/ch0..ch3 表格CSV。"""
    trial_map = _load_trial_metadata(path)
    with path.open("r", encoding="utf-8-sig", errors="ignore", newline="") as f:
        reader = csv.DictReader(f)
        names = {str(x).strip().lower() for x in (reader.fieldnames or [])}
        required = {"label", "win_id", "sample_idx", "ch0", "ch1", "ch2", "ch3"}
        if not required.issubset(names):
            return []

        grouped: Dict[int, Dict[int, Tuple[int, List[int]]]] = {}
        for row in reader:
            try:
                label = int(row["label"])
                win_id = int(row["win_id"])
                sample_idx = int(row["sample_idx"])
                values = [int(row[f"ch{i}"]) for i in range(4)]
            except (KeyError, TypeError, ValueError):
                continue
            if label not in LABEL_TO_CLASS or not (0 <= sample_idx < EXPECTED_SAMPLES):
                continue
            grouped.setdefault(win_id, {})[sample_idx] = (label, values)

    windows: List[CSPWindow] = []
    for win_id in sorted(grouped):
        rows = grouped[win_id]
        if len(rows) != EXPECTED_SAMPLES or any(i not in rows for i in range(EXPECTED_SAMPLES)):
            print(f"[跳过] {path.name} 窗口 {win_id} 不完整：{len(rows)}/256")
            continue
        labels = {rows[i][0] for i in range(EXPECTED_SAMPLES)}
        if len(labels) != 1:
            print(f"[跳过] {path.name} 窗口 {win_id} 标签不一致")
            continue
        label_raw = labels.pop()
        raw = [rows[i][1] for i in range(EXPECTED_SAMPLES)]
        windows.append(
            CSPWindow(
                data_uv=_prepare_window(raw),
                label=LABEL_TO_CLASS[label_raw],
                source_file=path.name,
                window_id=win_id,
                trial_hint=trial_map.get((label_raw, win_id)),
            )
        )
    return windows


def parse_serial_log(path: Path) -> List[CSPWindow]:
    """读取CSP_BEGIN/CSP/CSP_END原始串口日志。"""
    windows: List[CSPWindow] = []
    cur_label: Optional[int] = None
    cur_win: Optional[int] = None
    cur_trial: Optional[int] = None
    rows: Dict[int, List[int]] = {}

    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        line = raw_line.strip()
        if not line:
            continue

        begin = BEGIN_RE.search(line)
        if begin:
            cur_label = int(begin.group(1))
            cur_win = int(begin.group(2))
            trial_match = re.search(r"trial=(\d+)", line, re.IGNORECASE)
            cur_trial = int(trial_match.group(1)) if trial_match else None
            rows = {}
            continue

        item = ROW_RE.search(line)
        if item and cur_win is not None:
            win_id = int(item.group(1))
            sample_idx = int(item.group(2))
            if win_id == cur_win and 0 <= sample_idx < EXPECTED_SAMPLES:
                rows[sample_idx] = [int(item.group(i)) for i in range(3, 7)]
            continue

        end = END_RE.search(line)
        if end and cur_win is not None and cur_label is not None:
            end_label = int(end.group(1))
            end_win = int(end.group(2))
            complete = (
                end_label == cur_label
                and end_win == cur_win
                and cur_label in LABEL_TO_CLASS
                and len(rows) == EXPECTED_SAMPLES
                and all(i in rows for i in range(EXPECTED_SAMPLES))
            )
            if complete:
                raw = [rows[i] for i in range(EXPECTED_SAMPLES)]
                windows.append(
                    CSPWindow(
                        data_uv=_prepare_window(raw),
                        label=LABEL_TO_CLASS[cur_label],
                        source_file=path.name,
                        window_id=cur_win,
                        trial_hint=cur_trial,
                    )
                )
            else:
                print(f"[跳过] {path.name} 窗口 {cur_win} 不完整或首尾不匹配")
            cur_label = None
            cur_win = None
            cur_trial = None
            rows = {}

    return windows


def discover_files(data_dir: Path) -> List[Path]:
    if not data_dir.exists():
        raise RuntimeError(f"数据目录不存在：{data_dir}")
    files: List[Path] = []
    for pattern in ("*.csv", "*.txt", "*.log"):
        files.extend(data_dir.glob(pattern))
    skip = {"train_report.txt", "metrics.txt", "csp_features.csv", "model_search.csv"}
    return sorted({p for p in files if p.is_file() and p.name not in skip})


def load_windows(paths: Iterable[Path]) -> List[CSPWindow]:
    all_windows: List[CSPWindow] = []
    for path in paths:
        windows = parse_table_csv(path)
        source_type = "上位机CSV"
        if not windows:
            windows = parse_serial_log(path)
            source_type = "串口日志"
        if windows:
            print(f"[数据] {path.name}: {len(windows)} 个有效窗口（{source_type}）")
            all_windows.extend(windows)
        else:
            print(f"[警告] {path.name}: 未找到有效CSP窗口")
    if not all_windows:
        raise RuntimeError("没有读取到有效CSP窗口，请确认CSV列名或串口日志格式。")
    return all_windows


def assign_trial_groups(windows: List[CSPWindow]) -> None:
    """
    为验证建立独立试次分组。

    优先级：
    1. *_meta.json或串口trial字段；
    2. 无元数据时，标签变化即新试次；
    3. 即使连续出现同一标签，每累计4个窗口也自动开始新试次。
    """
    by_file: Dict[str, List[CSPWindow]] = {}
    for item in windows:
        by_file.setdefault(item.source_file, []).append(item)

    for source, items in by_file.items():
        items.sort(key=lambda x: x.window_id)
        inferred_trial = 0
        last_label: Optional[int] = None
        windows_in_trial = 0
        for item in items:
            if item.trial_hint is not None:
                item.trial_group = f"{source}:trial:{item.trial_hint}"
                last_label = item.label
                windows_in_trial = 0
                continue

            need_new = (
                last_label is None
                or item.label != last_label
                or windows_in_trial >= EXPECTED_WINDOWS_PER_TRIAL
            )
            if need_new:
                inferred_trial += 1
                windows_in_trial = 0
            item.trial_group = f"{source}:run:{inferred_trial}"
            last_label = item.label
            windows_in_trial += 1


def normalized_covariance(trial: np.ndarray, reg: float) -> np.ndarray:
    x = trial - trial.mean(axis=1, keepdims=True)
    cov = x @ x.T
    trace = float(np.trace(cov))
    if trace <= EPS:
        trace = 1.0
    cov = cov / trace
    if reg > 0.0:
        cov = (1.0 - reg) * cov + reg * np.eye(EXPECTED_CHANNELS) / EXPECTED_CHANNELS
    return cov


def fit_csp(X: np.ndarray, y: np.ndarray, reg: float) -> np.ndarray:
    left_cov = np.mean([normalized_covariance(x, reg) for x in X[y == 0]], axis=0)
    right_cov = np.mean([normalized_covariance(x, reg) for x in X[y == 1]], axis=0)
    composite = left_cov + right_cov + 1.0e-9 * np.eye(EXPECTED_CHANNELS)

    eigenvalues, eigenvectors = eigh(left_cov, composite)
    order = np.argsort(eigenvalues)[::-1]
    # 固定输出4行：最偏LEFT的两个分量 + 最偏RIGHT的两个分量。
    selected = [order[0], order[1], order[-2], order[-1]]
    W = eigenvectors[:, selected].T

    for i in range(W.shape[0]):
        norm = np.linalg.norm(W[i])
        if norm > 1.0e-12:
            W[i] /= norm
    return W.astype(np.float64)


def csp_features(X: np.ndarray, W: np.ndarray) -> np.ndarray:
    features: List[np.ndarray] = []
    for trial in X:
        z = W @ trial
        mean = z.mean(axis=1)
        var = np.mean(z * z, axis=1) - mean * mean
        var = np.maximum(var, EPS)
        total = max(float(np.sum(var)), EPS)
        features.append(np.log(var / total))
    return np.asarray(features, dtype=np.float64)


def feature_indices(mode: str) -> np.ndarray:
    if mode == "extreme2":
        return np.asarray([0, 3], dtype=np.int64)
    if mode == "all4":
        return np.asarray([0, 1, 2, 3], dtype=np.int64)
    raise ValueError(f"未知特征模式：{mode}")


def build_classifier(name: str, param: ParamValue):
    if name == "linear_svm":
        return LinearSVC(
            C=float(param),
            class_weight="balanced",
            max_iter=50000,
            random_state=RANDOM_STATE,
        )
    if name == "logistic":
        return LogisticRegression(
            C=float(param),
            class_weight="balanced",
            solver="liblinear",
            max_iter=5000,
            random_state=RANDOM_STATE,
        )
    if name == "lda":
        shrinkage: Union[str, float]
        shrinkage = "auto" if str(param) == "auto" else float(param)
        return LinearDiscriminantAnalysis(
            solver="lsqr",
            shrinkage=shrinkage,
            priors=np.asarray([0.5, 0.5]),
        )
    raise ValueError(f"未知分类器：{name}")


def fit_pipeline(X: np.ndarray, y: np.ndarray, candidate: Candidate) -> FittedPipeline:
    W = fit_csp(X, y, candidate.cov_reg)
    F = csp_features(X, W)
    scaler = StandardScaler()
    Fs = scaler.fit_transform(F)
    idx = feature_indices(candidate.feature_mode)
    clf = build_classifier(candidate.classifier, candidate.classifier_param)
    clf.fit(Fs[:, idx], y)
    return FittedPipeline(W, scaler, clf, idx, F)


def predict_pipeline(pipeline: FittedPipeline, X: np.ndarray) -> np.ndarray:
    F = csp_features(X, pipeline.csp_matrix)
    Fs = pipeline.scaler.transform(F)
    return np.asarray(pipeline.classifier.predict(Fs[:, pipeline.feature_indices]), dtype=np.int64)


def decision_pipeline(pipeline: FittedPipeline, X: np.ndarray) -> np.ndarray:
    F = csp_features(X, pipeline.csp_matrix)
    Fs = pipeline.scaler.transform(F)[:, pipeline.feature_indices]
    clf = pipeline.classifier
    if hasattr(clf, "decision_function"):
        return np.asarray(clf.decision_function(Fs), dtype=np.float64).reshape(-1)
    if hasattr(clf, "predict_proba"):
        proba = np.asarray(clf.predict_proba(Fs), dtype=np.float64)
        p = np.clip(proba[:, 1], 1.0e-8, 1.0 - 1.0e-8)
        return np.log(p / (1.0 - p))
    raise RuntimeError("分类器无法导出线性判别值")


def make_group_folds(y: np.ndarray, groups: np.ndarray) -> List[Tuple[np.ndarray, np.ndarray]]:
    """
    构造不拆分独立试次的验证折。
    试次较多时使用5折分组分层验证，避免候选模型搜索耗时过长；
    组数较少时采用留一组验证。
    """
    unique_groups = np.unique(groups)
    folds: List[Tuple[np.ndarray, np.ndarray]] = []
    if len(unique_groups) >= 6:
        splitter = StratifiedGroupKFold(
            n_splits=min(5, len(unique_groups)),
            shuffle=True,
            random_state=RANDOM_STATE,
        )
        for train_idx, test_idx in splitter.split(np.zeros(len(y)), y, groups):
            if len(np.unique(y[train_idx])) == 2 and len(test_idx) > 0:
                folds.append((train_idx, test_idx))
        return folds

    for group in unique_groups:
        test_idx = np.flatnonzero(groups == group)
        train_idx = np.flatnonzero(groups != group)
        if len(test_idx) > 0 and len(np.unique(y[train_idx])) == 2:
            folds.append((train_idx, test_idx))
    return folds


def evaluate_groups(
    X: np.ndarray,
    y: np.ndarray,
    groups: np.ndarray,
    candidate: Candidate,
) -> Tuple[float, float, np.ndarray]:
    folds = make_group_folds(y, groups)
    if not folds:
        return float("nan"), float("nan"), np.full_like(y, -1)

    pred = np.full_like(y, -1)
    for train_idx, test_idx in folds:
        try:
            pipeline = fit_pipeline(X[train_idx], y[train_idx], candidate)
            pred[test_idx] = predict_pipeline(pipeline, X[test_idx])
        except Exception:
            continue

    valid = pred >= 0
    if not np.any(valid) or len(np.unique(y[valid])) < 2:
        return float("nan"), float("nan"), pred
    score = float(balanced_accuracy_score(y[valid], pred[valid]))
    right_ratio = float(np.mean(pred[valid] == 1))
    return score, right_ratio, pred


def candidate_grid() -> List[Candidate]:
    rows: List[Candidate] = []
    for reg in (0.0, 0.05, 0.10, 0.20):
        for mode in ("extreme2", "all4"):
            for c_value in (0.1, 1.0, 10.0):
                rows.append(Candidate(reg, "linear_svm", c_value, mode))
                rows.append(Candidate(reg, "logistic", c_value, mode))
            for shrinkage in (0.0, 0.1, "auto"):
                rows.append(Candidate(reg, "lda", shrinkage, mode))
    return rows


def choose_candidate(
    X: np.ndarray,
    y: np.ndarray,
    trial_groups: np.ndarray,
    session_groups: np.ndarray,
):
    search_rows: List[dict] = []
    best_key = None
    best_payload = None
    has_multiple_sessions = len(np.unique(session_groups)) >= 2

    for candidate in candidate_grid():
        trial_score, trial_ratio, trial_pred = evaluate_groups(X, y, trial_groups, candidate)
        if has_multiple_sessions:
            session_score, session_ratio, session_pred = evaluate_groups(X, y, session_groups, candidate)
        else:
            session_score, session_ratio = float("nan"), float("nan")
            session_pred = np.full_like(y, -1)

        if np.isfinite(session_score) and np.isfinite(trial_score):
            base_score = 0.65 * session_score + 0.35 * trial_score
            ratio = session_ratio
        elif np.isfinite(session_score):
            base_score = session_score
            ratio = session_ratio
        elif np.isfinite(trial_score):
            base_score = trial_score
            ratio = trial_ratio
        else:
            base_score = -999.0
            ratio = float("nan")

        balance_penalty = abs(ratio - 0.5) if np.isfinite(ratio) else 1.0
        feature_penalty = 0.003 if candidate.feature_mode == "all4" else 0.0
        objective = base_score - 0.15 * balance_penalty - feature_penalty

        row = {
            "cov_reg": candidate.cov_reg,
            "classifier": candidate.classifier,
            "classifier_param": candidate.classifier_param,
            "feature_mode": candidate.feature_mode,
            "trial_cv_balanced_accuracy": trial_score,
            "trial_cv_pred_right_ratio": trial_ratio,
            "session_cv_balanced_accuracy": session_score,
            "session_cv_pred_right_ratio": session_ratio,
            "objective": objective,
        }
        search_rows.append(row)

        key = (
            objective,
            session_score if np.isfinite(session_score) else -1.0,
            trial_score if np.isfinite(trial_score) else -1.0,
            -balance_penalty,
            1 if candidate.feature_mode == "extreme2" else 0,
        )
        if best_key is None or key > best_key:
            best_key = key
            best_payload = (candidate, trial_pred, session_pred)

    if best_payload is None:
        raise RuntimeError("所有候选模型训练失败，请检查有效试次数量和数据质量。")
    return best_payload[0], best_payload[1], best_payload[2], search_rows


def extract_linear_parameters(pipeline: FittedPipeline, X: np.ndarray) -> Tuple[np.ndarray, float, float]:
    """提取线性参数，并扩展为固件固定的4维权重。"""
    clf = pipeline.classifier
    if not hasattr(clf, "coef_") or not hasattr(clf, "intercept_"):
        raise RuntimeError("所选分类器不是可部署的线性模型")

    coef_selected = np.asarray(clf.coef_, dtype=np.float64).reshape(-1)
    bias = float(np.asarray(clf.intercept_, dtype=np.float64).reshape(-1)[0])
    full_coef = np.zeros(EXPECTED_CHANNELS, dtype=np.float64)
    full_coef[pipeline.feature_indices] = coef_selected

    # 统一不同分类器的margin量级，使MCU端Sigmoid置信度不因LDA/SVM尺度差异而饱和。
    margins = decision_pipeline(pipeline, X)
    finite = np.abs(margins[np.isfinite(margins)])
    margin_scale = float(np.percentile(finite, 75)) if finite.size else 1.0
    margin_scale = max(1.0, min(margin_scale, 100.0))
    full_coef /= margin_scale
    bias /= margin_scale
    return full_coef, bias, margin_scale


def c_array_1d(name: str, values: np.ndarray) -> str:
    chunks = []
    flat = values.reshape(-1)
    for i in range(0, len(flat), 4):
        chunks.append("    " + ", ".join(f"{float(v):.9e}f" for v in flat[i : i + 4]) + ",")
    return f"static const float {name}[{len(flat)}] = {{\n" + "\n".join(chunks) + "\n};\n"


def c_array_2d(name: str, values: np.ndarray) -> str:
    lines = ["    {" + ", ".join(f"{float(v):.9e}f" for v in row) + "}," for row in values]
    return (
        f"static const float {name}[{values.shape[0]}][{values.shape[1]}] = {{\n"
        + "\n".join(lines)
        + "\n};\n"
    )


def export_header(
    path: Path,
    pipeline: FittedPipeline,
    candidate: Candidate,
    linear_weight: np.ndarray,
    linear_bias: float,
    margin_scale: float,
) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    active = ",".join(str(int(i)) for i in pipeline.feature_indices)

    text = f"""#ifndef __DUALCORE_CSP_MODEL_4CH_WEIGHTS_H
#define __DUALCORE_CSP_MODEL_4CH_WEIGHTS_H

/*
 * 由 python_csp/train_csp_lr.py 自动生成。
 * 输入通道顺序：CP3、CP4、C3、C4。
 * 固件标签：LEFT=2、RIGHT=3；模型类别：LEFT=0、RIGHT=1。
 * 特征公式：log(var(WX) / sum(var(WX)))。
 * 分类器：{candidate.classifier}，参数={candidate.classifier_param}。
 * 有效CSP特征索引：{active}；未使用维度的线性权重自动置0。
 * 为兼容现有V5F代码，所有线性分类器均导出为
 * csp4ch_svm_weight / csp4ch_svm_bias，变量名不代表一定使用SVM。
 * 生成时间：{stamp}
 */
#define CSP4CH_INPUT_CH             4
#define CSP4CH_FEATURE_DIM          4
#define CSP4CH_WEIGHTS_TRAINED      1
#define CSP4CH_COV_REG              {candidate.cov_reg:.6f}f
#define CSP4CH_ACTIVE_FEATURE_COUNT {len(pipeline.feature_indices)}
#define CSP4CH_MARGIN_SCALE         {margin_scale:.9e}f

"""
    text += c_array_2d("csp4ch_matrix", pipeline.csp_matrix)
    text += "\n" + c_array_1d("csp4ch_mean", pipeline.scaler.mean_.astype(np.float64))
    text += "\n" + c_array_1d("csp4ch_scale", pipeline.scaler.scale_.astype(np.float64))
    text += "\n" + c_array_1d("csp4ch_svm_weight", linear_weight)
    text += f"\nstatic const float csp4ch_svm_bias = {linear_bias:.9e}f;\n\n"
    text += "#endif /* __DUALCORE_CSP_MODEL_4CH_WEIGHTS_H */\n"
    path.write_text(text, encoding="utf-8")


def save_feature_csv(path: Path, features: np.ndarray, windows: Sequence[CSPWindow]) -> None:
    with path.open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["source_file", "trial_group", "window_id", "label", "csp0", "csp1", "csp2", "csp3"])
        for feature, item in zip(features, windows):
            writer.writerow([item.source_file, item.trial_group, item.window_id, CLASS_NAMES[item.label], *feature.tolist()])


def safe_metrics(y: np.ndarray, pred: np.ndarray):
    valid = pred >= 0
    if not np.any(valid) or len(np.unique(y[valid])) < 2:
        return float("nan"), float("nan"), np.zeros((2, 2), dtype=int)
    return (
        float(balanced_accuracy_score(y[valid], pred[valid])),
        float(np.mean(pred[valid] == 1)),
        confusion_matrix(y[valid], pred[valid], labels=[0, 1]),
    )


def main() -> None:
    script_dir = Path(__file__).resolve().parent
    # python_csp 位于上位机目录内；再向上一级才是包含 Common/V3F/V5F 的工程根目录。
    project_root = script_dir.parent.parent

    parser = argparse.ArgumentParser(description="四通道正则化CSP与线性分类器自动选择")
    parser.add_argument("--data-dir", type=Path, default=script_dir / "data")
    parser.add_argument("--log", type=Path, default=None,
                        help="只训练指定的一个CSV/TXT/LOG文件（兼容原命令行用法）")
    parser.add_argument("--out-dir", type=Path, default=script_dir / "output")
    parser.add_argument(
        "--firmware-header",
        type=Path,
        default=project_root / "Common" / "DualCore_CSP_Model_4ch_Weights.h",
    )
    parser.add_argument("--no-copy", action="store_true", help="只生成权重，不覆盖MCU工程")
    args = parser.parse_args()

    files = [args.log] if args.log is not None else discover_files(args.data_dir)
    if not files:
        raise RuntimeError(f"{args.data_dir} 中没有 .csv/.txt/.log 数据文件")

    windows = load_windows(files)
    assign_trial_groups(windows)

    X = np.stack([item.data_uv for item in windows], axis=0)
    y = np.asarray([item.label for item in windows], dtype=np.int64)
    trial_groups = np.asarray([item.trial_group for item in windows], dtype=object)
    session_groups = np.asarray([item.source_file for item in windows], dtype=object)

    if len(np.unique(y)) != 2:
        raise RuntimeError("CSP训练必须同时包含LEFT和RIGHT窗口")

    left_count = int(np.sum(y == 0))
    right_count = int(np.sum(y == 1))
    trial_count = len(np.unique(trial_groups))
    session_count = len(np.unique(session_groups))
    print(
        f"[汇总] 窗口={len(y)}，LEFT={left_count}，RIGHT={right_count}，"
        f"独立试次={trial_count}，文件/会话={session_count}"
    )

    selected, trial_pred, session_pred, search_rows = choose_candidate(
        X, y, trial_groups, session_groups
    )
    trial_bacc, trial_ratio, trial_cm = safe_metrics(y, trial_pred)
    session_bacc, session_ratio, session_cm = safe_metrics(y, session_pred)

    pipeline = fit_pipeline(X, y, selected)
    train_pred = predict_pipeline(pipeline, X)
    train_bacc = float(balanced_accuracy_score(y, train_pred))
    train_cm = confusion_matrix(y, train_pred, labels=[0, 1])
    linear_weight, linear_bias, margin_scale = extract_linear_parameters(pipeline, X)

    args.out_dir.mkdir(parents=True, exist_ok=True)
    generated_header = args.out_dir / "DualCore_CSP_Model_4ch_Weights.h"
    export_header(
        generated_header,
        pipeline,
        selected,
        linear_weight,
        linear_bias,
        margin_scale,
    )

    if not args.no_copy:
        args.firmware_header.parent.mkdir(parents=True, exist_ok=True)
        backup_dir = args.out_dir / "backup"
        backup_dir.mkdir(parents=True, exist_ok=True)
        if args.firmware_header.exists():
            stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            shutil.copy2(
                args.firmware_header,
                backup_dir / f"DualCore_CSP_Model_4ch_Weights_{stamp}.h",
            )
        shutil.copy2(generated_header, args.firmware_header)

    model_payload = {
        "csp_matrix": pipeline.csp_matrix,
        "scaler": pipeline.scaler,
        "classifier": pipeline.classifier,
        "feature_indices": pipeline.feature_indices,
        "candidate": selected,
        "margin_scale": margin_scale,
        "channel_order": ["CP3", "CP4", "C3", "C4"],
    }
    joblib.dump(model_payload, args.out_dir / "trained_csp_linear.joblib")
    # 保留旧文件名，避免原有使用习惯失效。
    joblib.dump(model_payload, args.out_dir / "trained_csp_svm.joblib")
    save_feature_csv(args.out_dir / "csp_features.csv", pipeline.raw_features, windows)

    with (args.out_dir / "model_search.csv").open("w", encoding="utf-8-sig", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(search_rows[0].keys()))
        writer.writeheader()
        writer.writerows(search_rows)

    metrics = {
        "window_count": int(len(y)),
        "left_count": left_count,
        "right_count": right_count,
        "trial_group_count": trial_count,
        "session_file_count": session_count,
        "selected_cov_reg": selected.cov_reg,
        "selected_classifier": selected.classifier,
        "selected_classifier_param": selected.classifier_param,
        "selected_feature_mode": selected.feature_mode,
        "selected_feature_indices": pipeline.feature_indices.tolist(),
        "margin_scale": margin_scale,
        "trial_cv_balanced_accuracy": trial_bacc,
        "trial_cv_pred_right_ratio": trial_ratio,
        "trial_cv_confusion_matrix": trial_cm.tolist(),
        "session_cv_balanced_accuracy": session_bacc,
        "session_cv_pred_right_ratio": session_ratio,
        "session_cv_confusion_matrix": session_cm.tolist(),
        "train_balanced_accuracy": train_bacc,
        "train_confusion_matrix": train_cm.tolist(),
        "firmware_header": str(args.firmware_header),
    }
    (args.out_dir / "metrics.json").write_text(
        json.dumps(metrics, ensure_ascii=False, indent=2), encoding="utf-8"
    )

    report = [
        "四通道正则化CSP + 线性分类器自动选择训练报告",
        "=" * 58,
        f"数据文件数：{len(files)}",
        f"有效窗口：{len(y)}",
        f"LEFT窗口：{left_count}",
        f"RIGHT窗口：{right_count}",
        f"独立试次组：{trial_count}",
        f"文件/会话数：{session_count}",
        "",
        f"最终分类器：{selected.classifier}",
        f"分类器参数：{selected.classifier_param}",
        f"CSP协方差正则：{selected.cov_reg}",
        f"CSP特征模式：{selected.feature_mode}",
        f"实际使用特征索引：{pipeline.feature_indices.tolist()}",
        f"导出margin缩放：{margin_scale:.6f}",
        "",
        f"按独立试次验证平衡准确率：{trial_bacc:.4f}" if np.isfinite(trial_bacc) else "按独立试次验证：试次数不足",
        f"按独立试次验证RIGHT比例：{trial_ratio:.4f}" if np.isfinite(trial_ratio) else "按独立试次验证RIGHT比例：无",
        "按独立试次验证混淆矩阵（行真值LEFT/RIGHT，列预测LEFT/RIGHT）：",
        str(trial_cm),
        "",
        f"跨文件/跨会话验证平衡准确率：{session_bacc:.4f}" if np.isfinite(session_bacc) else "跨文件/跨会话验证：至少需要两个可用数据文件",
        f"跨文件验证RIGHT比例：{session_ratio:.4f}" if np.isfinite(session_ratio) else "跨文件验证RIGHT比例：无",
        "跨文件验证混淆矩阵：",
        str(session_cm),
        "",
        f"全数据训练平衡准确率：{train_bacc:.4f}",
        "训练集混淆矩阵：",
        str(train_cm),
        "",
        f"生成权重：{generated_header}",
        f"MCU权重：{args.firmware_header}" if not args.no_copy else "未自动覆盖MCU权重（--no-copy）",
        "",
        "说明：无论最终选择SVM、LDA还是逻辑回归，均导出为原V5F可直接使用的4维weight+bias。",
        "下一步：Clean并Build V5F，然后重新烧录V5F。",
    ]
    report_text = "\n".join(report)
    (args.out_dir / "metrics.txt").write_text(report_text, encoding="utf-8")
    print(report_text)


if __name__ == "__main__":
    main()
