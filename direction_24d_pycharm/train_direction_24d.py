# -*- coding: utf-8 -*-
"""
24维 LEFT/RIGHT 方向识别训练脚本（PyCharm 直接运行版）

用法：
1. 把串口保存的 .txt / .csv / .log 文件放到 data/ 文件夹。
2. PyCharm 直接运行本文件，或命令行运行：
       python train_direction_24d.py
3. 训练结果输出到 output/，MCU 可替换文件输出到 mcu_export/。

默认标签规则：
- DIRCSV,2,...  -> LEFT
- DIRCSV,3,...  -> RIGHT
- DIRCSV,label,f1,...,f24 形式会自动识别前 24 个特征
- DIRCSV,0,...  -> LEFT
- DIRCSV,1,...  -> RIGHT
- 行内或文件名包含 LEFT/RIGHT 也会识别。
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import os
import re
import shutil
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, List, Optional, Sequence, Tuple

import joblib
import numpy as np
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.model_selection import StratifiedKFold, cross_val_score, train_test_split
from sklearn.neural_network import MLPClassifier
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.svm import LinearSVC

LEFT = 0
RIGHT = 1
LABEL_NAMES = {LEFT: "LEFT", RIGHT: "RIGHT"}


@dataclass
class ParsedDataset:
    X: np.ndarray
    y: np.ndarray
    sources: List[str]


def _try_float(token: str) -> Optional[float]:
    token = token.strip()
    if not token:
        return None
    # 去掉可能附着在 token 上的单位或等号前缀，例如 F0=123.4
    if "=" in token:
        token = token.split("=", 1)[1].strip()
    token = token.strip("[](){}")
    try:
        value = float(token)
    except ValueError:
        return None
    if math.isfinite(value):
        return value
    return None


def _label_from_text(text: str) -> Optional[int]:
    t = text.upper()
    # 避免 RIGHT 中包含 I 等误判，这里用边界匹配
    if re.search(r"(^|[^A-Z])LEFT([^A-Z]|$)", t) or re.search(r"(^|[^A-Z])L([^A-Z]|$)", t):
        return LEFT
    if re.search(r"(^|[^A-Z])RIGHT([^A-Z]|$)", t) or re.search(r"(^|[^A-Z])R([^A-Z]|$)", t):
        return RIGHT
    return None


def _label_from_numeric(value: float) -> Optional[int]:
    iv = int(round(value))
    # 兼容你工程里常见的 DIRCSV 标签：2=LEFT, 3=RIGHT
    if iv in (0, 2):
        return LEFT
    if iv in (1, 3):
        return RIGHT
    return None


def _split_line(line: str) -> List[str]:
    # 同时兼容逗号、空格、制表符、中文逗号
    line = line.replace("，", ",")
    return [p.strip() for p in re.split(r"[,\t ]+", line.strip()) if p.strip()]


def parse_feature_line(line: str, filename_label: Optional[int], feature_dim: int) -> Optional[Tuple[np.ndarray, int]]:
    raw = line.strip()
    if not raw:
        return None
    if raw.startswith("#") or raw.startswith("//"):
        return None
    upper = raw.upper()
    if upper.startswith(("TASK=", "STATUS", "MODE", "TRIAL", "CSP_BEGIN", "CSP_END", "CSP,")):
        return None
    if "HEADER" in upper:
        return None

    parts = _split_line(raw)
    if not parts:
        return None

    label: Optional[int] = _label_from_text(raw)
    feature_tokens: List[str] = []

    # 优先识别 DIRCSV,label,f1,f2...
    if parts[0].upper().startswith("DIRCSV"):
        if len(parts) < 2:
            return None
        lab_f = _try_float(parts[1])
        if lab_f is not None:
            label = _label_from_numeric(lab_f)
        elif label is None:
            label = _label_from_text(parts[1])
        feature_tokens = parts[2:]
    else:
        # 兼容 label,f1,f2... 或 f1,f2... + 文件名带标签
        first_f = _try_float(parts[0])
        if label is None and first_f is not None:
            maybe_label = _label_from_numeric(first_f)
            # 如果剩余数字数量刚好足够，认为第一列是标签
            if maybe_label is not None:
                label = maybe_label
                feature_tokens = parts[1:]
            else:
                feature_tokens = parts
        else:
            feature_tokens = parts

    if label is None:
        label = filename_label
    if label is None:
        return None

    nums: List[float] = []
    for token in feature_tokens:
        val = _try_float(token)
        if val is not None:
            nums.append(val)

    if len(nums) < feature_dim:
        return None
    if len(nums) > feature_dim:
        # 保留前 feature_dim 维，兼容旧数据行里后面附带调试字段的情况
        nums = nums[:feature_dim]

    return np.asarray(nums, dtype=np.float32), label


def collect_data_files(data_dir: Path, explicit_logs: Sequence[str]) -> List[Path]:
    if explicit_logs:
        return [Path(p).resolve() for p in explicit_logs]
    exts = {".txt", ".csv", ".log"}
    files = sorted([p for p in data_dir.rglob("*") if p.is_file() and p.suffix.lower() in exts])
    return files


def load_dataset(data_dir: Path, logs: Sequence[str], feature_dim: int) -> ParsedDataset:
    files = collect_data_files(data_dir, logs)
    if not files:
        raise FileNotFoundError(f"没有找到训练数据文件。请把 .txt/.csv/.log 放到：{data_dir}")

    X_list: List[np.ndarray] = []
    y_list: List[int] = []
    sources: List[str] = []

    for path in files:
        filename_label = _label_from_text(path.stem)
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except UnicodeDecodeError:
            text = path.read_text(encoding="gbk", errors="ignore")
        for line_no, line in enumerate(text.splitlines(), start=1):
            parsed = parse_feature_line(line, filename_label, feature_dim)
            if parsed is None:
                continue
            feat, label = parsed
            X_list.append(feat)
            y_list.append(label)
            sources.append(f"{path.name}:{line_no}")

    if not X_list:
        raise ValueError("找到了文件，但没有解析到有效的 24 维 LEFT/RIGHT 数据。请检查 DIRCSV 行或文件名标签。")

    X = np.vstack(X_list).astype(np.float32)
    y = np.asarray(y_list, dtype=np.int32)
    return ParsedDataset(X=X, y=y, sources=sources)


def train_models(X: np.ndarray, y: np.ndarray, seed: int, hidden_dim: int):
    scaler = StandardScaler()
    Xs = scaler.fit_transform(X)

    svm = LinearSVC(class_weight="balanced", C=1.0, max_iter=50000, random_state=seed)
    svm.fit(Xs, y)

    lda = LinearDiscriminantAnalysis(solver="lsqr", shrinkage="auto")
    lda.fit(Xs, y)

    mlp = MLPClassifier(
        hidden_layer_sizes=(hidden_dim,),
        activation="relu",
        solver="adam",
        alpha=1e-4,
        batch_size="auto",
        learning_rate_init=1e-3,
        max_iter=3000,
        early_stopping=True,
        validation_fraction=0.15,
        n_iter_no_change=80,
        random_state=seed,
    )
    mlp.fit(Xs, y)

    return scaler, svm, lda, mlp


def safe_train_test_report(X: np.ndarray, y: np.ndarray, seed: int, hidden_dim: int) -> str:
    lines: List[str] = []
    n_left = int(np.sum(y == LEFT))
    n_right = int(np.sum(y == RIGHT))
    lines.append(f"样本数: {len(y)}")
    lines.append(f"LEFT: {n_left}, RIGHT: {n_right}")
    lines.append(f"特征维度: {X.shape[1]}")

    if len(np.unique(y)) < 2:
        lines.append("错误：只检测到一个类别，无法训练二分类模型。")
        return "\n".join(lines)

    min_class = min(n_left, n_right)
    if min_class >= 4 and len(y) >= 10:
        test_size = 0.25 if len(y) >= 20 else 0.33
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=test_size, random_state=seed, stratify=y
        )
        scaler, svm, lda, mlp = train_models(X_train, y_train, seed, hidden_dim)
        Xts = scaler.transform(X_test)
        for name, model in [("Linear SVM", svm), ("LDA", lda), ("MLP", mlp)]:
            pred = model.predict(Xts)
            lines.append("")
            lines.append(f"[{name}] holdout accuracy = {accuracy_score(y_test, pred):.4f}")
            lines.append(str(confusion_matrix(y_test, pred, labels=[LEFT, RIGHT])))
            lines.append(classification_report(y_test, pred, target_names=["LEFT", "RIGHT"], zero_division=0))

    if min_class >= 5:
        folds = min(5, min_class)
        skf = StratifiedKFold(n_splits=folds, shuffle=True, random_state=seed)
        candidates = {
            "Linear SVM": make_pipeline(StandardScaler(), LinearSVC(class_weight="balanced", C=1.0, max_iter=50000, random_state=seed)),
            "LDA": make_pipeline(StandardScaler(), LinearDiscriminantAnalysis(solver="lsqr", shrinkage="auto")),
            "MLP": make_pipeline(StandardScaler(), MLPClassifier(hidden_layer_sizes=(hidden_dim,), max_iter=2500, early_stopping=True, random_state=seed)),
        }
        lines.append("")
        lines.append(f"{folds}折交叉验证：")
        for name, pipe in candidates.items():
            try:
                scores = cross_val_score(pipe, X, y, cv=skf, scoring="accuracy")
                lines.append(f"{name}: mean={scores.mean():.4f}, std={scores.std():.4f}, scores={np.array2string(scores, precision=3)}")
            except Exception as exc:
                lines.append(f"{name}: 交叉验证失败：{exc}")

    return "\n".join(lines)


def c_array(name: str, arr: np.ndarray, per_line: int = 8) -> str:
    flat = np.asarray(arr, dtype=np.float32).reshape(-1)
    lines = [f"static const float {name}[{flat.size}] = {{"]
    for i in range(0, flat.size, per_line):
        chunk = flat[i:i + per_line]
        vals = ", ".join(f"{float(v):.8e}f" for v in chunk)
        lines.append(f"    {vals},")
    lines.append("};")
    return "\n".join(lines)


def export_weights_h(path: Path, scaler: StandardScaler, svm: LinearSVC, lda: LinearDiscriminantAnalysis, mlp: MLPClassifier,
                     feature_dim: int, hidden_dim: int, default_model: str):
    mean = scaler.mean_.astype(np.float32)
    scale = scaler.scale_.astype(np.float32)
    scale[np.abs(scale) < 1e-6] = 1.0

    # LinearSVC: classes are [0,1], decision_function > 0 means RIGHT.
    svm_w = svm.coef_.reshape(-1).astype(np.float32)
    svm_b = float(svm.intercept_[0])

    lda_w = lda.coef_.reshape(-1).astype(np.float32)
    lda_b = float(lda.intercept_[0])

    w0 = mlp.coefs_[0].astype(np.float32).reshape(-1)
    b0 = mlp.intercepts_[0].astype(np.float32)
    w1 = mlp.coefs_[1].astype(np.float32).reshape(-1)
    b1 = mlp.intercepts_[1].astype(np.float32)

    text = f"""#ifndef __DIRECTION_MODEL_4CH_WEIGHTS_H
#define __DIRECTION_MODEL_4CH_WEIGHTS_H

/* Auto-generated by train_direction_24d.py.
 * Dataset: 24-dim LEFT/RIGHT DIRCSV feature dataset.
 * Python labels: LEFT=0, RIGHT=1. MCU prints LEFT/RIGHT.
 * Default classifier selected in Direction_Classifier.h: {default_model.upper()}.
 */

#define DIR4CH_INPUT_DIM {feature_dim}
#define DIR4CH_WEIGHTS_TRAINED 1
#define DIR4CH_MLP_H1_DIM {hidden_dim}

{c_array('dir4ch_mean', mean)}

{c_array('dir4ch_scale', scale)}

{c_array('dir4ch_svm_weight', svm_w)}

static const float dir4ch_svm_bias = {svm_b:.8e}f;

{c_array('dir4ch_lda_weight', lda_w)}

static const float dir4ch_lda_bias = {lda_b:.8e}f;

{c_array('dir4ch_mlp_w0', w0)}

{c_array('dir4ch_mlp_b0', b0)}

{c_array('dir4ch_mlp_w1', w1)}

{c_array('dir4ch_mlp_b1', b1)}

#endif /* __DIRECTION_MODEL_4CH_WEIGHTS_H */
"""
    path.write_text(text, encoding="utf-8")


def export_mcu_files(export_dir: Path, weights_h: Path, feature_dim: int, default_model: str):
    export_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy2(weights_h, export_dir / "Direction_Model_4ch_Weights.h")

    model_type_define = {
        "lda": "DIR_CLASSIFIER_MODEL_LDA",
        "svm": "DIR_CLASSIFIER_MODEL_LINEAR_SVM",
        "mlp": "DIR_CLASSIFIER_MODEL_MLP",
    }.get(default_model.lower(), "DIR_CLASSIFIER_MODEL_LINEAR_SVM")

    (export_dir / "Direction_Classifier.h").write_text(f"""#ifndef __DIRECTION_CLASSIFIER_H
#define __DIRECTION_CLASSIFIER_H

#ifdef __cplusplus
extern "C" {{
#endif

#include <stdint.h>

#define DIR_CLASSIFIER_INPUT_DIM {feature_dim}
#define DIR_CLASSIFIER_CLASS_NUM 2

#define DIR_CLASSIFIER_MODEL_LDA        1
#define DIR_CLASSIFIER_MODEL_LINEAR_SVM 2
#define DIR_CLASSIFIER_MODEL_MLP        3

#ifndef DIR_CLASSIFIER_MODEL_TYPE
#define DIR_CLASSIFIER_MODEL_TYPE {model_type_define}
#endif

#ifndef DIR_CLASSIFIER_ENABLE_UNKNOWN
#define DIR_CLASSIFIER_ENABLE_UNKNOWN 1
#endif

#ifndef DIR_CLASSIFIER_CONF_THRESHOLD
#define DIR_CLASSIFIER_CONF_THRESHOLD 800
#endif

#ifndef DIR_CLASSIFIER_LINEAR_SCORE_GAIN
#define DIR_CLASSIFIER_LINEAR_SCORE_GAIN 1.0f
#endif

typedef enum {{
    DIR_CLS_LEFT = 0,
    DIR_CLS_RIGHT = 1,
    DIR_CLS_UNKNOWN = 2
}} DirectionClassifierIntent_t;

typedef struct {{
    DirectionClassifierIntent_t intent;
    int32_t score_left;      /* 0~10000 */
    int32_t score_right;     /* 0~10000 */
    int32_t confidence;      /* abs(score_left-score_right) */
    uint8_t trained;
}} DirectionClassifierResult_t;

DirectionClassifierIntent_t Direction_Classifier_Predict(const float feature[DIR_CLASSIFIER_INPUT_DIM],
                                                          DirectionClassifierResult_t *result);
const char *Direction_Classifier_ToString(DirectionClassifierIntent_t intent);
const char *Direction_Classifier_ModelName(void);
uint8_t Direction_Classifier_IsTrained(void);

#ifdef __cplusplus
}}
#endif

#endif /* __DIRECTION_CLASSIFIER_H */
""", encoding="utf-8")

    (export_dir / "Direction_Classifier.c").write_text(r'''#include "Direction_Classifier.h"
#include "Direction_Model_4ch_Weights.h"
#include <math.h>

static float dir_relu(float x)
{
    return (x > 0.0f) ? x : 0.0f;
}

static float dir_sigmoid(float x)
{
    if (x > 30.0f) return 1.0f;
    if (x < -30.0f) return 0.0f;
    return 1.0f / (1.0f + expf(-x));
}

static float dir_std_feature(const float feature[DIR_CLASSIFIER_INPUT_DIM], int i)
{
    float scale = dir4ch_scale[i];
    if (scale < 1.0e-6f && scale > -1.0e-6f) {
        scale = 1.0f;
    }
    return (feature[i] - dir4ch_mean[i]) / scale;
}

static float dir_linear_raw_score(const float feature[DIR_CLASSIFIER_INPUT_DIM],
                                  const float *w,
                                  float b)
{
    float s = b;
    for (int i = 0; i < DIR_CLASSIFIER_INPUT_DIM; i++) {
        s += w[i] * dir_std_feature(feature, i);
    }
    return s;
}

static float dir_svm_right_probability(const float feature[DIR_CLASSIFIER_INPUT_DIM])
{
    float margin = dir_linear_raw_score(feature, dir4ch_svm_weight, dir4ch_svm_bias);
    return dir_sigmoid(margin * DIR_CLASSIFIER_LINEAR_SCORE_GAIN);
}

static float dir_lda_right_probability(const float feature[DIR_CLASSIFIER_INPUT_DIM])
{
    float margin = dir_linear_raw_score(feature, dir4ch_lda_weight, dir4ch_lda_bias);
    return dir_sigmoid(margin * DIR_CLASSIFIER_LINEAR_SCORE_GAIN);
}

static float dir_mlp_right_probability(const float feature[DIR_CLASSIFIER_INPUT_DIM])
{
    float h[DIR4CH_MLP_H1_DIM];
    float z;

    for (int j = 0; j < DIR4CH_MLP_H1_DIM; j++) {
        float s = dir4ch_mlp_b0[j];
        for (int i = 0; i < DIR_CLASSIFIER_INPUT_DIM; i++) {
            s += dir_std_feature(feature, i) * dir4ch_mlp_w0[i * DIR4CH_MLP_H1_DIM + j];
        }
        h[j] = dir_relu(s);
    }

    z = dir4ch_mlp_b1[0];
    for (int j = 0; j < DIR4CH_MLP_H1_DIM; j++) {
        z += h[j] * dir4ch_mlp_w1[j];
    }
    return dir_sigmoid(z);
}

DirectionClassifierIntent_t Direction_Classifier_Predict(const float feature[DIR_CLASSIFIER_INPUT_DIM],
                                                          DirectionClassifierResult_t *result)
{
    float p_right;
    int32_t sr;
    int32_t sl;
    int32_t conf;
    DirectionClassifierIntent_t intent;

#if DIR_CLASSIFIER_MODEL_TYPE == DIR_CLASSIFIER_MODEL_LDA
    p_right = dir_lda_right_probability(feature);
#elif DIR_CLASSIFIER_MODEL_TYPE == DIR_CLASSIFIER_MODEL_MLP
    p_right = dir_mlp_right_probability(feature);
#else
    p_right = dir_svm_right_probability(feature);
#endif

    sr = (int32_t)(p_right * 10000.0f + 0.5f);
    if (sr < 0) sr = 0;
    if (sr > 10000) sr = 10000;
    sl = 10000 - sr;
    conf = sl - sr;
    if (conf < 0) conf = -conf;

    if (p_right >= 0.5f) {
        intent = DIR_CLS_RIGHT;
    } else {
        intent = DIR_CLS_LEFT;
    }

#if DIR_CLASSIFIER_ENABLE_UNKNOWN
    if (conf < DIR_CLASSIFIER_CONF_THRESHOLD) {
        intent = DIR_CLS_UNKNOWN;
    }
#endif

    if (result != 0) {
        result->intent = intent;
        result->score_left = sl;
        result->score_right = sr;
        result->confidence = conf;
        result->trained = Direction_Classifier_IsTrained();
    }
    return intent;
}

const char *Direction_Classifier_ToString(DirectionClassifierIntent_t intent)
{
    switch (intent) {
    case DIR_CLS_LEFT: return "LEFT";
    case DIR_CLS_RIGHT: return "RIGHT";
    default: return "UNKNOWN";
    }
}

const char *Direction_Classifier_ModelName(void)
{
#if DIR_CLASSIFIER_MODEL_TYPE == DIR_CLASSIFIER_MODEL_LDA
    return "LDA";
#elif DIR_CLASSIFIER_MODEL_TYPE == DIR_CLASSIFIER_MODEL_MLP
    return "MLP16";
#else
    return "LINEAR_SVM";
#endif
}

uint8_t Direction_Classifier_IsTrained(void)
{
#ifdef DIR4CH_WEIGHTS_TRAINED
    return DIR4CH_WEIGHTS_TRAINED ? 1 : 0;
#else
    return 1;
#endif
}
''', encoding="utf-8")

    (export_dir / "Direction_Model.h").write_text('''#ifndef __DIRECTION_MODEL_H
#define __DIRECTION_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define DIR_CLASS_NUM 2

typedef enum {
    DIRECTION_LEFT = 0,
    DIRECTION_RIGHT = 1,
    DIRECTION_UNKNOWN = 2
} Direction_t;

Direction_t Direction_Model_Infer(const float feature[24], int32_t score[DIR_CLASS_NUM]);
const char *Direction_ToString(Direction_t dir);
uint8_t Direction_Model_IsTrained(void);
const char *Direction_Model_Name(void);

#ifdef __cplusplus
}
#endif

#endif /* __DIRECTION_MODEL_H */
''', encoding="utf-8")

    (export_dir / "Direction_Model.c").write_text('''#include "Direction_Model.h"
#include "Direction_Classifier.h"

static Direction_t convert_intent(DirectionClassifierIntent_t intent)
{
    switch (intent) {
    case DIR_CLS_LEFT:
        return DIRECTION_LEFT;
    case DIR_CLS_RIGHT:
        return DIRECTION_RIGHT;
    default:
        return DIRECTION_UNKNOWN;
    }
}

Direction_t Direction_Model_Infer(const float feature[24], int32_t score[DIR_CLASS_NUM])
{
    DirectionClassifierResult_t result;
    DirectionClassifierIntent_t intent = Direction_Classifier_Predict(feature, &result);

    if (score != 0) {
        score[0] = result.score_left;
        score[1] = result.score_right;
    }
    return convert_intent(intent);
}

const char *Direction_ToString(Direction_t dir)
{
    switch (dir) {
    case DIRECTION_LEFT:
        return "LEFT";
    case DIRECTION_RIGHT:
        return "RIGHT";
    default:
        return "UNKNOWN";
    }
}

uint8_t Direction_Model_IsTrained(void)
{
    return Direction_Classifier_IsTrained();
}

const char *Direction_Model_Name(void)
{
    return Direction_Classifier_ModelName();
}
''', encoding="utf-8")


def save_confusion_csv(path: Path, y_true: np.ndarray, y_pred: np.ndarray):
    cm = confusion_matrix(y_true, y_pred, labels=[LEFT, RIGHT])
    with path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["", "pred_LEFT", "pred_RIGHT"])
        writer.writerow(["true_LEFT", int(cm[0, 0]), int(cm[0, 1])])
        writer.writerow(["true_RIGHT", int(cm[1, 0]), int(cm[1, 1])])


def main(argv: Optional[Sequence[str]] = None) -> int:
    root = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(description="Train 24-dim LEFT/RIGHT direction classifier and export MCU weights.")
    parser.add_argument("--data-dir", default=str(root / "data"), help="训练数据目录，默认 ./data")
    parser.add_argument("--log", action="append", default=[], help="手动指定一个或多个训练数据文件；不指定则自动读取 data/ 下所有 txt/csv/log")
    parser.add_argument("--feature-dim", type=int, default=24, help="特征维度，默认 24")
    parser.add_argument("--hidden", type=int, default=16, help="MLP 隐层神经元数量，默认 16")
    parser.add_argument("--model", choices=["svm", "lda", "mlp"], default="svm", help="MCU 默认启用模型，默认 svm；也可以改为 mlp 或 lda")
    parser.add_argument("--seed", type=int, default=42)
    args = parser.parse_args(argv)

    data_dir = Path(args.data_dir).resolve()
    out_dir = root / "output"
    export_dir = root / "mcu_export"
    out_dir.mkdir(exist_ok=True)
    export_dir.mkdir(exist_ok=True)

    ds = load_dataset(data_dir, args.log, args.feature_dim)
    X, y = ds.X, ds.y

    report = safe_train_test_report(X, y, args.seed, args.hidden)
    print(report)
    (out_dir / "metrics.txt").write_text(report + "\n", encoding="utf-8")

    scaler, svm, lda, mlp = train_models(X, y, args.seed, args.hidden)

    # 全量训练后的训练集预测，仅用于保存一个最终混淆矩阵参考
    Xs = scaler.transform(X)
    default_model = {"svm": svm, "lda": lda, "mlp": mlp}[args.model]
    pred = default_model.predict(Xs)
    save_confusion_csv(out_dir / "confusion_matrix_train.csv", y, pred)

    model_bundle = {
        "feature_dim": args.feature_dim,
        "hidden_dim": args.hidden,
        "default_model": args.model,
        "scaler": scaler,
        "svm": svm,
        "lda": lda,
        "mlp": mlp,
        "label_names": LABEL_NAMES,
    }
    joblib.dump(model_bundle, out_dir / "trained_direction_24d.joblib")

    weights_path = out_dir / "Direction_Model_4ch_Weights.h"
    export_weights_h(weights_path, scaler, svm, lda, mlp, args.feature_dim, args.hidden, args.model)
    export_mcu_files(export_dir, weights_path, args.feature_dim, args.model)

    meta = {
        "feature_dim": args.feature_dim,
        "sample_count": int(len(y)),
        "left_count": int(np.sum(y == LEFT)),
        "right_count": int(np.sum(y == RIGHT)),
        "default_model": args.model,
        "hidden_dim": args.hidden,
        "data_dir": str(data_dir),
    }
    (out_dir / "train_meta.json").write_text(json.dumps(meta, ensure_ascii=False, indent=2), encoding="utf-8")

    print("\n训练完成。")
    print(f"权重头文件: {weights_path}")
    print(f"MCU 可替换文件目录: {export_dir}")
    print("替换 MCU 文件后，请 Clean V3F / Build V3F / Clean V5F / Build V5F / 下载 V5F。")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
