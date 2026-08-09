#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""比较shrinkage中间变量"""

import csv
import numpy as np
from sklearn.preprocessing import StandardScaler
from pathlib import Path

FEATURE_DIM = 24
MODEL_FEATURE_DIM = 14

def normalize_label(raw):
    if raw in (2, 0): return 0
    if raw in (3, 1): return 1
    return None

data_dir = Path(__file__).parent / "data"
rows = []
for path in sorted(data_dir.glob("train_DIRCSV_*.csv")):
    with path.open("r", encoding="utf-8-sig", errors="ignore", newline="") as h:
        reader = csv.DictReader(h)
        for r in reader:
            try:
                label = normalize_label(int(r["label"]))
            except (ValueError, KeyError):
                continue
            if label is None:
                continue
            feats = []
            for i in range(FEATURE_DIM):
                try:
                    feats.append(float(r[f"feat{i}"]))
                except (ValueError, KeyError):
                    feats = None
                    break
            if feats is None:
                continue
            rows.append((feats[:MODEL_FEATURE_DIM], label))

X = np.array([r[0] for r in rows], dtype=np.float64)
y = np.array([r[1] for r in rows])
n, d = X.shape

scaler = StandardScaler()
X_std = scaler.fit_transform(X)

# sklearn ledoit_wolf_shrinkage internals
X2 = X_std ** 2
emp_cov_trace = np.sum(X2, axis=0) / n
mu = np.sum(emp_cov_trace) / d

beta_raw = np.sum(np.dot(X2.T, X2))
delta_raw = np.sum(np.dot(X_std.T, X_std) ** 2)

beta_ = beta_raw
delta_ = delta_raw / n**2

beta = 1. / (d * n) * (beta_ / n - delta_)
delta = delta_ / d - mu**2

print(f"n={n} d={d}")
print(f"mu = {mu}")
print(f"beta_raw = {beta_raw}")
print(f"delta_raw = {delta_raw}")
print(f"beta_ (no /n^2) = {beta_}")
print(f"delta_ (after /n^2) = {delta_}")
print(f"beta_ / n = {beta_ / n}")
print(f"beta = {beta}")
print(f"delta = {delta}")
print(f"shrinkage = {min(beta, delta) / delta}")

# Also compute using my Android formula
beta_android = 0.0
for i in range(n):
    row_sq = np.sum(X_std[i] ** 2)
    beta_android += row_sq * row_sq
print(f"\n--- Android formula ---")
print(f"beta_raw (sum ||x_i||^4) = {beta_android}")

beta_val = 1.0 / (d * n) * (beta_android / n - delta_)
print(f"beta = {beta_val}")
print(f"shrinkage = {min(beta_val, delta) / delta}")
