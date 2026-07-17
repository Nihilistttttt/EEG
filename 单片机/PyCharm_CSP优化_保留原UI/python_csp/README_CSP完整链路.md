# CSP采集、训练与推理使用说明

## 1. 单片机采集

上位机选择 `CSP` 模式后点击训练。单片机输出：

```text
CSP_BEGIN,label=2,win=1,trial=1,fs=250,n=256,ch=4,unit=uVx1000,order=CP3_CP4_C3_C4,source=V3F_FILTERED
CSP,1,0,...
...
CSP_END,label=2,win=1,trial=1
```

采集窗口来自 `CP3、CP4、C3、C4` 四通道，已经完成实时慢漂移去除、50Hz陷波和 `2–40 Hz` 带通滤波。V3F与V5F的CSP链路使用相同滤波系数。

请使用 `DEBUG` 串口采集文本数据，波特率为 `921600`。WiFi串口同时承载二进制数据帧，不建议直接作为CSP文本采集口。

## 2. 数据放置

上位机保存的：

```text
train_CSP_时间.csv
```

放入：

```text
python_csp/data/
```

CSV格式保持为：

```text
label,win_id,sample_idx,ch0,ch1,ch2,ch3
```

也可以直接把包含 `CSP_BEGIN/CSP/CSP_END` 的 `.txt` 或 `.log` 放入该目录。

## 3. 训练

第一次双击 `安装依赖.bat`，之后双击：

```text
一键训练CSP并替换权重.bat
```

脚本会：

1. 自动读取data目录全部CSV/TXT/LOG；
2. 检查每个窗口是否完整包含256点；
3. 按连续标签段推断独立试次，避免重叠窗口泄漏；
4. 训练CSP空间滤波器；
5. 训练并搜索Linear SVM参数；
6. 输出按试次验证结果；
7. 备份并覆盖真实固件工程中的 `Common/DualCore_CSP_Model_4ch_Weights.h`。

## 4. 烧录

训练完成后：

```text
Clean V5F
Build V5F
重新烧录V5F
```

V3F原有波形、频谱、注意力、姿态和24维训练功能均未删除。

## 5. 在Python界面切换推理模型

- 选择 `COLLECT` 并点击“应用”：自动发送 `MODEL,SET,FFT24`；之后点击“测试”使用24维模型。
- 选择 `CSP` 并点击“应用”：自动发送 `MODEL,SET,CSP`；之后点击“测试”使用CSP模型。
- 进入 `INFER` 只改变运行状态，不会覆盖前面选定的模型。
- 若CSP权重尚未训练，固件仍会安全回退FFT24，界面会显示 `实际: FFT24`。

## 6. 判断CSP是否生效

推理输出末尾会出现：

```text
MODEL=CSP
```

表示V5F正在使用CSP+Linear SVM。

若出现：

```text
MODEL=FFT24
```

表示CSP权重尚未训练，系统自动回退到原有24维FFT+SVM模型。
