# EEG双核脑电采集与SSVEP训练系统 — 项目上下文

> 本文档为AI协作开发提供完整项目上下文。详细技术细节见 AI_CONTEXT_TEMPLATE.txt

---

## 一、项目概述

基于CH32H417双核MCU（V3F+V5F）的EEG采集与SSVEP方向识别系统：
- **V3F核**：ADS1299采集、专注度/放松度算法、OLED/串口/WiFi
- **V5F核**：Goertzel频谱、24维特征、SVM/CSP方向推理
- **Android端**：医生端(训练控制+波形) + 患者端(被动接收)

---

## 二、当前状态

### 已完成（全部验证通过）
- 双核IPC通信完全打通（ack≈notify, ok=100%）
- V3F推理移除，INFER模式统一使用V5F推理结果
- 专注度/放松度算法全面重写（P0-P5, A1-A4）
- IIR系数/状态分离（4通道共享系数，节省54%内存）
- FFT静态分配（消除malloc碎片风险）
- logf查表+线性插值（3-5x加速，修复偏移bug）
- Signal_Analysis.c拆分为8模块，dualcore_ipc.c拆分为6模块
- STOP命令修复 + ResetInferState
- doctor/patient从android分支同步（SSVEP/MI训练分离）

### 待做（诚实评估后）
1. **全通道伪迹检测**（CH2/CH3无保护，是正确性bug，2行代码）
2. **专注度SVM分类器**（长期最大收益，需训练数据）
3. IPCDIAG频率可配置（锦上添花）

### 已否决
- 增量均值/斜率更新（N=10遍历仅50 cycles，无可增量化的热点）
- 自适应Notch（工频偏移极罕见，Goertzel不受干扰）
- 动态参数配置（运行时调参需求不存在）
- V3F/V5F重复代码提取（改两个.cproject风险>收益）
- HSEM互斥保护（3次尝试均卡死，已放弃）

---

## 三、关键架构决策

| 决策 | 选择 | 理由 |
|------|------|------|
| V3F推理 | 已移除，统一V5F | 避免双核结果矛盾，V3F专注采集+专注度 |
| FFT内存 | 静态分配 | 消除malloc碎片风险，448KB共享RAM充裕 |
| logf实现 | 查表+线性插值 | 3-5x加速，误差<1.2e-6 |
| IIR架构 | 系数共享+状态独立 | 4通道同滤波器系数相同，节省54%内存 |
| HSEM互斥 | 放弃，用FENCE | 3次尝试均卡死，设计规避已足够 |
| 专注度公式 | 多特征融合 | 0.45*log(β/(α+θ))+0.35*log(β/α)+0.20*log(δ/θ) |

---

## 四、Git信息

- 仓库: https://github.com/Nihilistttttt/EEG.git
- 最新commit: 71b5cbd (从android分支同步doctor/patient)
- 推送需: `git -c http.sslVerify=false push`
- android分支拉取: `git fetch origin android && git checkout origin/android -- doctor patient`

---

## 五、关键文件速查

| 文件 | 用途 | 行数 |
|------|------|------|
| Common/dualcore_ipc_shared.h | 共享常量/类型(IIR_Coeff_t等) | ~100 |
| Common/dualcore_v5f_dsp.c | V5F DSP算法链 | ~610 |
| V3F/Attention/algo_core.c | 专注度算法核心 | ~450 |
| V3F/EEG_FFT/eeg_fft.c | FFT状态机+专注度融合 | ~630 |
| V3F/EEG_Filter/eeg_filter.c | IIR滤波器+去趋势 | ~140 |
| V3F/Hardware/Signal_Analysis.c | 主调度器 | ~202 |
