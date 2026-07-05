# 24维 LEFT/RIGHT 方向识别训练脚本（PyCharm 版）

这个文件夹用于你当前工程保留的 **24维 FFT / 频段特征算法**。
它不是 CSP 原始窗口训练脚本，也不是 20维数据集脚本。

## 1. 数据放哪里

把串口助手保存的数据文件放到：

```text
data/
```

文件名随便，例如：

```text
data/left_right_01.txt
data/train_20260702.log
data/my_data.csv
```

脚本会自动读取 `data` 文件夹下所有 `.txt / .csv / .log` 文件。

## 2. 支持的数据格式

推荐格式：

```text
DIRCSV,2,f1,f2,...,f24
DIRCSV,3,f1,f2,...,f24
```

标签规则：

```text
2 或 0 -> LEFT
3 或 1 -> RIGHT
```

如果文件名包含 LEFT 或 RIGHT，也能辅助识别标签。
如果一行后面有多余调试字段，脚本会只取前 24 个特征。

## 3. PyCharm 运行方式

在 PyCharm 里打开本文件夹，然后安装依赖：

```bash
pip install -r requirements.txt
```

直接运行：

```bash
python train_direction_24d.py
```

也可以手动指定文件：

```bash
python train_direction_24d.py --log data/my_data.txt
```

## 4. 输出文件

训练完成后会生成：

```text
output/metrics.txt
output/train_meta.json
output/trained_direction_24d.joblib
output/Direction_Model_4ch_Weights.h
mcu_export/Direction_Classifier.c
mcu_export/Direction_Classifier.h
mcu_export/Direction_Model.c
mcu_export/Direction_Model.h
mcu_export/Direction_Model_4ch_Weights.h
```

## 5. 替换到 MCU 工程

把 `mcu_export` 里的 5 个文件覆盖到当前 MCU 工程的同名文件。
替换后必须：

```text
Clean V3F
Build V3F
Clean V5F
Build V5F
选中 V5F 下载
```

因为现在推理在 V5F 上跑，只编译 V3F 不会更新推理权重。

## 6. 默认模型

脚本会同时训练：

```text
Linear SVM
LDA
MLP16
```

默认导出的 MCU 模型类型是：

```text
Linear SVM
```

如果想默认用 MLP：

```bash
python train_direction_24d.py --model mlp
```

如果想默认用 LDA：

```bash
python train_direction_24d.py --model lda
```

注意：不管默认选择哪个，权重文件里会同时包含 SVM / LDA / MLP 的权重。
