#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""训练V5F空间窄带14维收缩LDA，并按原24列协议导出MCU权重。"""

from __future__ import annotations

import argparse
import csv
import json
import re
import shutil
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple

import joblib
import numpy as np
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.metrics import balanced_accuracy_score, confusion_matrix
from sklearn.model_selection import LeaveOneGroupOut, StratifiedGroupKFold
from sklearn.preprocessing import StandardScaler


FEATURE_DIM = 24
MODEL_FEATURE_DIM = 14
LEFT_RAW = 2
RIGHT_RAW = 3
LEFT_CLASS = 0
RIGHT_CLASS = 1
RANDOM_STATE = 17
DIRCSV_RE = re.compile(r"DIRCSV,(\d+),(.+)$", re.IGNORECASE)


@dataclass
class FeatureRow:
    values: np.ndarray
    label: int
    source_file: str
    row_index: int
    trial_hint: Optional[int] = None
    trial_group: str = ""


def normalize_label(raw: int) -> Optional[int]:
    if raw in (LEFT_RAW, LEFT_CLASS):
        return LEFT_CLASS
    if raw in (RIGHT_RAW, RIGHT_CLASS):
        return RIGHT_CLASS
    return None


def load_metadata(path: Path) -> Dict[int, int]:
    meta_path = path.with_name(path.stem + "_meta.json")
    if not meta_path.exists():
        return {}
    try:
        payload = json.loads(meta_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        print(f"[警告] 无法读取 {meta_path.name}: {exc}")
        return {}

    result: Dict[int, int] = {}
    for item in payload.get("rows", []):
        try:
            index = int(item["row_index"])
            trial_id = int(item["trial_id"])
        except (KeyError, TypeError, ValueError):
            continue
        result[index] = trial_id
    if result:
        print(f"[元数据] {meta_path.name}: {len(result)} 行具有试次编号")
    return result


def parse_feature_values(items: Sequence[object]) -> Optional[np.ndarray]:
    try:
        values = np.asarray([float(item) for item in items], dtype=np.float64)
    except (TypeError, ValueError):
        return None
    if values.shape != (FEATURE_DIM,) or not np.all(np.isfinite(values)):
        return None
    return values


def parse_table_csv(path: Path) -> List[FeatureRow]:
    trial_map = load_metadata(path)
    rows: List[FeatureRow] = []
    with path.open("r", encoding="utf-8-sig", errors="ignore", newline="") as handle:
        reader = csv.DictReader(handle)
        actual = {str(name).strip().lower(): name for name in (reader.fieldnames or [])}
        needed = ["label"] + [f"feat{i}" for i in range(FEATURE_DIM)]
        if not all(name in actual for name in needed):
            return []

        for source_index, item in enumerate(reader):
            try:
                raw_label = int(str(item[actual["label"]]).strip())
            except (TypeError, ValueError):
                continue
            label = normalize_label(raw_label)
            values = parse_feature_values(
                [item[actual[f"feat{i}"]] for i in range(FEATURE_DIM)]
            )
            if label is None or values is None:
                continue
            rows.append(
                FeatureRow(
                    values=values,
                    label=label,
                    source_file=path.name,
                    row_index=len(rows),
                    trial_hint=trial_map.get(source_index),
                )
            )
    return rows


def parse_serial_log(path: Path) -> List[FeatureRow]:
    rows: List[FeatureRow] = []
    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        match = DIRCSV_RE.search(raw_line.strip())
        if not match:
            continue
        label = normalize_label(int(match.group(1)))
        values = parse_feature_values([part.strip() for part in match.group(2).split(",")])
        if label is None or values is None:
            continue
        rows.append(
            FeatureRow(
                values=values,
                label=label,
                source_file=path.name,
                row_index=len(rows),
            )
        )
    return rows


# ========== 二进制协议日志解析（与 MCU eeg_protocol.h 对齐） ==========
PROTO_AA = 0xAA
PROTO_55 = 0x55
PROTO_TAIL = 0x7E
PROTO_ESC = 0x7D
PROTO_XOR = 0x20
PROTO_BODY_BASE = 1 + 1 + 2 + 4  # addr + cmd + len(2) + ts(4)
CMD_DIRCSV = 0x26
DIRCSV_FEAT_BYTES = 24 * 4


def _try_handle_dircsv_frame(body: bytes, name: str, rows: List[FeatureRow]) -> None:
    if len(body) < PROTO_BODY_BASE + 2:
        return
    payload_len = body[2] | (body[3] << 8)
    if len(body) != PROTO_BODY_BASE + payload_len + 2:
        return
    calc = sum(body[: PROTO_BODY_BASE + payload_len]) & 0xFFFF
    got = body[PROTO_BODY_BASE + payload_len] | (
        body[PROTO_BODY_BASE + payload_len + 1] << 8
    )
    if calc != got:
        return
    if body[1] != CMD_DIRCSV:
        return
    if payload_len != 1 + DIRCSV_FEAT_BYTES:
        return
    off = PROTO_BODY_BASE
    label = normalize_label(body[off])
    if label is None:
        return
    values = np.frombuffer(
        bytes(body[off + 1: off + 1 + DIRCSV_FEAT_BYTES]), dtype="<i4"
    ).astype(np.float64)
    if values.shape != (FEATURE_DIM,) or not np.all(np.isfinite(values)):
        return
    rows.append(
        FeatureRow(
            values=values,
            label=label,
            source_file=name,
            row_index=len(rows),
        )
    )


def parse_binary_log(path: Path) -> List[FeatureRow]:
    rows: List[FeatureRow] = []
    try:
        data = path.read_bytes()
    except OSError:
        return rows
    state = 0
    body = bytearray()
    for b in data:
        if state == 0:
            if b == PROTO_AA:
                state = 1
        elif state == 1:
            if b == PROTO_55:
                state = 2
                body = bytearray()
            elif b == PROTO_AA:
                state = 1
            else:
                state = 0
        elif state == 2:
            if b == PROTO_ESC:
                state = 3
            elif b == PROTO_TAIL:
                _try_handle_dircsv_frame(bytes(body), path.name, rows)
                state = 0
            else:
                body.append(b)
        elif state == 3:
            body.append(b ^ PROTO_XOR)
            state = 2
    return rows


def load_file(path: Path) -> List[FeatureRow]:
    rows = parse_table_csv(path) if path.suffix.lower() == ".csv" else []
    if not rows:
        rows = parse_serial_log(path)
    if not rows:
        rows = parse_binary_log(path)
    if rows:
        print(f"[读取] {path.name}: 有效24维行={len(rows)}")
    else:
        print(f"[跳过] {path.name}: 不是有效的24维DIRCSV数据")
    return rows


def discover_files(data_dir: Path) -> List[Path]:
    if not data_dir.is_dir():
        return []
    return sorted(
        path for path in data_dir.iterdir()
        if path.is_file() and path.suffix.lower() in {".csv", ".txt", ".log"}
    )


def assign_trial_groups(rows: List[FeatureRow]) -> None:
    by_source: Dict[str, List[FeatureRow]] = {}
    for row in rows:
        by_source.setdefault(row.source_file, []).append(row)

    for source, source_rows in by_source.items():
        source_rows.sort(key=lambda item: item.row_index)
        inferred_run = -1
        previous_label: Optional[int] = None
        for row in source_rows:
            if row.trial_hint is not None:
                row.trial_group = f"{source}::meta::{row.trial_hint}"
                previous_label = row.label
                continue
            if previous_label is None or row.label != previous_label:
                inferred_run += 1
            row.trial_group = f"{source}::run::{inferred_run}"
            previous_label = row.label


def fit_model(
    X: np.ndarray,
    y: np.ndarray,
) -> Tuple[StandardScaler, LinearDiscriminantAnalysis]:
    scaler = StandardScaler()
    X_std = scaler.fit_transform(X)
    model = LinearDiscriminantAnalysis(
        solver="lsqr",
        shrinkage="auto",
        priors=np.asarray([0.5, 0.5], dtype=np.float64),
    )
    model.fit(X_std, y)
    return scaler, model


def model_features(X: np.ndarray) -> np.ndarray:
    """协议保留24列，模型只使用固件定义的前14个空间窄带特征。"""
    values = np.asarray(X, dtype=np.float64)
    if values.ndim != 2 or values.shape[1] != FEATURE_DIM:
        raise ValueError(f"期望{FEATURE_DIM}列特征，实际形状={values.shape}")
    return values[:, :MODEL_FEATURE_DIM]


def padded_mcu_parameters(
    scaler: StandardScaler,
    model: LinearDiscriminantAnalysis,
) -> Tuple[np.ndarray, np.ndarray, np.ndarray, float]:
    """把14维模型补成原固件24元素数组，后10维恒不参与推理。"""
    mean = np.zeros(FEATURE_DIM, dtype=np.float64)
    scale = np.ones(FEATURE_DIM, dtype=np.float64)
    weight = np.zeros(FEATURE_DIM, dtype=np.float64)
    mean[:MODEL_FEATURE_DIM] = np.asarray(scaler.mean_, dtype=np.float64)
    scale[:MODEL_FEATURE_DIM] = np.asarray(scaler.scale_, dtype=np.float64)
    weight[:MODEL_FEATURE_DIM] = np.asarray(model.coef_[0], dtype=np.float64)
    return mean, scale, weight, float(model.intercept_[0])


def metrics(y_true: np.ndarray, y_pred: np.ndarray) -> Tuple[float, float, np.ndarray]:
    bacc = float(balanced_accuracy_score(y_true, y_pred))
    right_ratio = float(np.mean(y_pred == RIGHT_CLASS))
    matrix = confusion_matrix(y_true, y_pred, labels=[LEFT_CLASS, RIGHT_CLASS])
    return bacc, right_ratio, matrix


def grouped_predictions(
    X: np.ndarray,
    y: np.ndarray,
    groups: np.ndarray,
    splitter: object,
) -> np.ndarray:
    predicted = np.full(len(y), -1, dtype=np.int64)
    for train_index, test_index in splitter.split(X, y, groups):
        if len(np.unique(y[train_index])) != 2:
            continue
        scaler, model = fit_model(X[train_index], y[train_index])
        predicted[test_index] = model.predict(scaler.transform(X[test_index]))
    return predicted


def trial_predictions(
    X: np.ndarray,
    y: np.ndarray,
    trial_groups: np.ndarray,
) -> np.ndarray:
    groups_per_class = [
        len(set(trial_groups[y == class_id].tolist()))
        for class_id in (LEFT_CLASS, RIGHT_CLASS)
    ]
    split_count = min(5, min(groups_per_class))
    if split_count < 2:
        raise RuntimeError("每一类至少需要2个独立试次；请完成LEFT/RIGHT/LEFT/RIGHT四组采集")

    splitter = StratifiedGroupKFold(
        n_splits=split_count,
        shuffle=True,
        random_state=RANDOM_STATE,
    )
    predicted = grouped_predictions(X, y, trial_groups, splitter)
    if not np.all(predicted >= 0):
        raise RuntimeError("无法完成按试次交叉验证，请检查试次标签和数据数量")
    return predicted


def session_predictions(
    X: np.ndarray,
    y: np.ndarray,
    session_groups: np.ndarray,
) -> Optional[np.ndarray]:
    if len(np.unique(session_groups)) < 2:
        return None
    predicted = grouped_predictions(X, y, session_groups, LeaveOneGroupOut())
    return predicted if np.all(predicted >= 0) else None


def c_float(value: float) -> str:
    if not np.isfinite(value):
        raise ValueError("权重包含NaN或Inf")
    return f"{float(value):.9e}f"


def c_array(name: str, values: np.ndarray) -> str:
    flat = np.asarray(values, dtype=np.float64).reshape(-1)
    lines = []
    for start in range(0, len(flat), 6):
        lines.append("    " + ", ".join(c_float(item) for item in flat[start:start + 6]) + ",")
    return f"static const float {name}[{len(flat)}] = {{\n" + "\n".join(lines) + "\n};"


def export_header(
    path: Path,
    scaler: StandardScaler,
    model: LinearDiscriminantAnalysis,
) -> None:
    mean, scale, weight, bias = padded_mcu_parameters(scaler, model)
    content = "\n".join(
        [
            "#ifndef __DIRECTION_MODEL_4CH_WEIGHTS_H",
            "#define __DIRECTION_MODEL_4CH_WEIGHTS_H",
            "",
            "/* Auto-generated by train_direction_24d.py.",
            " * Input protocol: 24 integer features carried by DIRCSV.",
            " * Model input: feat0..feat13 spatial narrow-band features.",
            " * feat14..feat23 are protocol padding and have zero weights.",
            " * Labels: LEFT=0 and RIGHT=1 internally; firmware DIRCSV uses 2/3.",
            " * Model: 14D spatial narrow-band shrinkage LDA (solver=lsqr, shrinkage=auto).",
            " * Positive decision value means RIGHT.",
            " * Legacy svm variable names are retained so V5F firmware stays unchanged.",
            " */",
            "",
            "#define DIR4CH_INPUT_DIM 24",
            "#define DIR4CH_WEIGHTS_TRAINED 1",
            "",
            c_array("dir4ch_mean", mean),
            "",
            c_array("dir4ch_scale", scale),
            "",
            c_array("dir4ch_svm_weight", weight),
            "",
            f"static const float dir4ch_svm_bias = {c_float(bias)};",
            "",
            "#endif /* __DIRECTION_MODEL_4CH_WEIGHTS_H */",
            "",
        ]
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="ascii")


def main() -> None:
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent.parent

    parser = argparse.ArgumentParser(description="训练并导出V5F空间窄带14维/24列协议收缩LDA权重")
    parser.add_argument("--data-dir", type=Path, default=script_dir / "data")
    parser.add_argument("--log", type=Path, default=None, help="只训练指定CSV/TXT/LOG")
    parser.add_argument("--out-dir", type=Path, default=script_dir / "output")
    parser.add_argument(
        "--firmware-header",
        type=Path,
        default=project_root / "Common" / "DualCore_Direction_Model_4ch_Weights.h",
    )
    parser.add_argument("--no-copy", action="store_true", help="不覆盖固件正式权重")
    args = parser.parse_args()

    files = [args.log] if args.log is not None else discover_files(args.data_dir)
    files = [Path(path) for path in files if path is not None and Path(path).is_file()]
    if not files:
        raise RuntimeError(f"{args.data_dir} 中没有CSV/TXT/LOG训练数据")

    rows: List[FeatureRow] = []
    for path in files:
        rows.extend(load_file(path))
    if not rows:
        raise RuntimeError("没有有效24维数据；CSP的4通道时域CSV不能用于FFT24训练")

    assign_trial_groups(rows)
    X = np.stack([row.values for row in rows], axis=0)
    old_schema_rows = int(np.sum(np.any(X[:, MODEL_FEATURE_DIM:] != 0.0, axis=1)))
    if old_schema_rows:
        raise RuntimeError(
            f"检测到{old_schema_rows}行旧版24维宽频带数据（feat14..feat23非零）。"
            "请清空本工具data目录，烧录本版V5F后重新采集；禁止新旧特征混训。"
        )
    X_model = model_features(X)
    y = np.asarray([row.label for row in rows], dtype=np.int64)
    trial_groups = np.asarray([row.trial_group for row in rows], dtype=object)
    session_groups = np.asarray([row.source_file for row in rows], dtype=object)

    if len(np.unique(y)) != 2:
        raise RuntimeError("必须同时包含LEFT(label=2)和RIGHT(label=3)")
    left_count = int(np.sum(y == LEFT_CLASS))
    right_count = int(np.sum(y == RIGHT_CLASS))
    trial_count = len(np.unique(trial_groups))
    session_count = len(np.unique(session_groups))
    print(
        f"[汇总] 行数={len(y)} LEFT={left_count} RIGHT={right_count} "
        f"独立试次={trial_count} 文件/会话={session_count}"
    )

    trial_pred = trial_predictions(X_model, y, trial_groups)
    trial_bacc, trial_ratio, trial_matrix = metrics(y, trial_pred)
    quality_warnings: List[str] = []
    if min(
        len(set(trial_groups[y == LEFT_CLASS].tolist())),
        len(set(trial_groups[y == RIGHT_CLASS].tolist())),
    ) < 10:
        quality_warnings.append(
            "每类独立试次少于10个；建议至少完成5次四组COLLECT采集，再训练正式展示权重"
        )
    if session_count < 2:
        quality_warnings.append("只有一个采集文件，无法完成更可靠的跨文件验证")
    if trial_bacc < 0.60:
        quality_warnings.append("按试次验证平衡准确率低于0.60，建议重新采集更多独立试次")
    if not 0.30 <= trial_ratio <= 0.70:
        quality_warnings.append("验证预测明显偏向单一方向，请检查左右标签、电极和动作执行")
    session_pred = session_predictions(X_model, y, session_groups)
    if session_pred is not None:
        session_bacc, session_ratio, session_matrix = metrics(y, session_pred)
    else:
        session_bacc, session_ratio, session_matrix = float("nan"), float("nan"), np.zeros((2, 2), dtype=int)

    scaler, model = fit_model(X_model, y)
    train_pred = model.predict(scaler.transform(X_model))
    train_bacc, train_ratio, train_matrix = metrics(y, train_pred)

    # 与MCU逐项公式自检：((x-mean)/scale)·weight+bias。
    python_margin = model.decision_function(scaler.transform(X_model))
    mean, scale, weight, bias = padded_mcu_parameters(scaler, model)
    mcu_margin = ((X - mean) / scale) @ weight + bias
    max_margin_error = float(np.max(np.abs(python_margin - mcu_margin)))
    if max_margin_error > 1.0e-9:
        raise RuntimeError(f"MCU公式自检失败，最大margin误差={max_margin_error}")

    # 模拟MCU的float32逐项累加，确认量化后左右判定仍与Python一致。
    mean32 = np.asarray(mean, dtype=np.float32)
    scale32 = np.asarray(scale, dtype=np.float32)
    weight32 = np.asarray(weight, dtype=np.float32)
    bias32 = np.float32(bias)
    X32 = np.asarray(X, dtype=np.float32)
    mcu_margin32 = np.empty(len(X32), dtype=np.float32)
    for row_index, row in enumerate(X32):
        value = np.float32(bias32)
        for feature_index in range(FEATURE_DIM):
            standardized = np.float32(
                (row[feature_index] - mean32[feature_index]) / scale32[feature_index]
            )
            value = np.float32(
                value + np.float32(weight32[feature_index] * standardized)
            )
        mcu_margin32[row_index] = value
    max_margin_error_float32 = float(
        np.max(np.abs(python_margin - mcu_margin32.astype(np.float64)))
    )
    prediction_mismatch_float32 = int(
        np.sum((python_margin >= 0.0) != (mcu_margin32 >= 0.0))
    )
    if prediction_mismatch_float32 != 0:
        raise RuntimeError(
            f"MCU float32判定自检失败，不一致={prediction_mismatch_float32}/{len(y)}"
        )

    args.out_dir.mkdir(parents=True, exist_ok=True)
    generated_header = args.out_dir / "DualCore_Direction_Model_4ch_Weights.h"
    export_header(generated_header, scaler, model)

    if not args.no_copy:
        args.firmware_header.parent.mkdir(parents=True, exist_ok=True)
        backup_dir = args.out_dir / "backup"
        backup_dir.mkdir(parents=True, exist_ok=True)
        if args.firmware_header.exists():
            stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            shutil.copy2(
                args.firmware_header,
                backup_dir / f"DualCore_Direction_Model_4ch_Weights_{stamp}.h",
            )
        shutil.copy2(generated_header, args.firmware_header)

    joblib.dump(
        {
            "scaler": scaler,
            "classifier": model,
            "model_type": "spatial_narrow14_shrinkage_lda",
            "solver": "lsqr",
            "shrinkage": "auto",
            "feature_dim": FEATURE_DIM,
            "model_feature_dim": MODEL_FEATURE_DIM,
            "model_feature_columns": list(range(MODEL_FEATURE_DIM)),
            "protocol_padding_columns": list(range(MODEL_FEATURE_DIM, FEATURE_DIM)),
            "class_names": {LEFT_CLASS: "LEFT", RIGHT_CLASS: "RIGHT"},
        },
        args.out_dir / "trained_fft24_lda.joblib",
    )

    validation_row = {
        "model": "spatial_narrow14_shrinkage_lda",
        "solver": "lsqr",
        "shrinkage": "auto",
        "model_feature_dim": MODEL_FEATURE_DIM,
        "trial_cv_balanced_accuracy": trial_bacc,
        "trial_cv_pred_right_ratio": trial_ratio,
        "tn": int(trial_matrix[0, 0]),
        "fp": int(trial_matrix[0, 1]),
        "fn": int(trial_matrix[1, 0]),
        "tp": int(trial_matrix[1, 1]),
    }
    with (args.out_dir / "model_search.csv").open("w", encoding="utf-8-sig", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(validation_row.keys()))
        writer.writeheader()
        writer.writerow(validation_row)

    report_data = {
        "row_count": len(y),
        "left_count": left_count,
        "right_count": right_count,
        "trial_group_count": trial_count,
        "session_file_count": session_count,
        "model_type": "spatial_narrow14_shrinkage_lda",
        "solver": "lsqr",
        "shrinkage": "auto",
        "protocol_feature_dim": FEATURE_DIM,
        "model_feature_dim": MODEL_FEATURE_DIM,
        "trial_cv_balanced_accuracy": trial_bacc,
        "trial_cv_pred_right_ratio": trial_ratio,
        "trial_cv_confusion_matrix": trial_matrix.tolist(),
        "quality_warnings": quality_warnings,
        "session_cv_balanced_accuracy": session_bacc if np.isfinite(session_bacc) else None,
        "session_cv_pred_right_ratio": session_ratio if np.isfinite(session_ratio) else None,
        "session_cv_confusion_matrix": session_matrix.tolist() if session_pred is not None else None,
        "train_balanced_accuracy": train_bacc,
        "train_pred_right_ratio": train_ratio,
        "train_confusion_matrix": train_matrix.tolist(),
        "mcu_margin_max_abs_error": max_margin_error,
        "mcu_float32_margin_max_abs_error": max_margin_error_float32,
        "mcu_float32_prediction_mismatch_count": prediction_mismatch_float32,
        "generated_header": str(generated_header),
        "firmware_header": str(args.firmware_header) if not args.no_copy else None,
    }
    (args.out_dir / "metrics.json").write_text(
        json.dumps(report_data, ensure_ascii=False, indent=2), encoding="utf-8"
    )

    report = [
        "24列协议/14维空间窄带特征 + 收缩LDA训练报告",
        "=" * 52,
        f"数据行：{len(y)}（LEFT={left_count}, RIGHT={right_count}）",
        f"独立试次：{trial_count}",
        f"文件/会话：{session_count}",
        "模型：前14维空间窄带 Shrinkage LDA（solver=lsqr, shrinkage=auto）",
        "协议：DIRCSV仍为24列，feat14..feat23不参与训练或推理",
        "",
        f"按试次验证平衡准确率：{trial_bacc:.4f}",
        f"按试次验证RIGHT比例：{trial_ratio:.4f}",
        "按试次混淆矩阵（行真值LEFT/RIGHT，列预测LEFT/RIGHT）：",
        str(trial_matrix),
        *(["", "数据质量警告："] + [f"- {item}" for item in quality_warnings] if quality_warnings else []),
        "",
        f"跨文件验证平衡准确率：{session_bacc:.4f}" if np.isfinite(session_bacc) else "跨文件验证：至少需要两个完整采集文件",
        f"跨文件验证RIGHT比例：{session_ratio:.4f}" if np.isfinite(session_ratio) else "跨文件验证RIGHT比例：无",
        "",
        f"全数据训练平衡准确率：{train_bacc:.4f}",
        f"MCU公式最大margin误差：{max_margin_error:.3e}",
        f"MCU float32最大margin误差：{max_margin_error_float32:.3e}",
        f"MCU float32左右判定不一致：{prediction_mismatch_float32}/{len(y)}",
        f"生成权重：{generated_header}",
        f"固件权重：{args.firmware_header}" if not args.no_copy else "正式固件权重未覆盖（--no-copy）",
        "",
        "下一步：在MounRiver中Clean/Build V5F并重新烧录V5F。",
    ]
    report_text = "\n".join(report)
    (args.out_dir / "metrics.txt").write_text(report_text, encoding="utf-8")
    print(report_text)


if __name__ == "__main__":
    main()
