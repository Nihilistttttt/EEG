# EEG双核脑电采集与SSVEP训练系统 — 项目上下文

> 本文档为AI协作开发提供完整项目上下文，涵盖架构、当前状态、开发历史和协作规范。

---

## 一、项目概述

### 目标
基于CH32H417双核微控制器（V3F+V5F）构建脑电波（EEG）采集与SSVEP方向识别训练系统：
- **V3F核**：ADS1299 8通道EEG采集、ICM42605 IMU、OLED显示、串口/WiFi通信、自身推理
- **V5F核**：FFT特征提取、CSP/方向分类器推理、结果回传V3F
- **Android端**：实时波形显示、训练流程控制、模型参数下发

### 硬件平台
- MCU: CH32H417 (RISC-V, V3F 100MHz + V5F 100MHz, 双核共享RAM)
- ADC: ADS1299 (8通道24位EEG前端, SPI接口, 250/500/1000 SPS可配)
- IMU: ICM42605 (6轴, SPI)
- 显示: 0.96" OLED (SPI)
- 通信: USART DEBUG + USART WiFi(ESP8266)
- 调试: SWD + 串口

### 软件架构
```
V3F (采集+外设主控)          V5F (信号处理+推理后端)
┌─────────────────┐          ┌─────────────────┐
│ ADS1299 SPI+DMA │          │                 │
│ Ring Buffer     │──IPC──→  │ FFT(256pt)      │
│ V3F自身推理      │  通知    │ 特征提取(24维)   │
│ OLED/Serial     │  ←ACK──  │ CSP/方向分类器   │
│ WiFi/ESP8266    │          │ 结果写共享slot   │
└─────────────────┘          └─────────────────┘
        │                            │
        └──── 共享RAM (0x20110000~0x2017FFFF) ────┘
              64-slot帧环形缓冲区
```

---

## 二、代码结构

```
EEG/
├── EXAM/
│   ├── GPIO/EEG/                    ← 主项目目录
│   │   ├── Common/
│   │   │   ├── dualcore_ipc.c       ← IPC核心实现(1297行)
│   │   │   └── dualcore_ipc.h       ← IPC公共接口
│   │   ├── V3F/
│   │   │   ├── Hardware/
│   │   │   │   ├── Signal_Analysis.c ← 采集主循环+推理(2450行)
│   │   │   │   ├── ADS1299.c/h       ← EEG ADC驱动
│   │   │   │   ├── ICM42605.c/h      ← IMU驱动
│   │   │   │   ├── Serial.c/h        ← 多端口串口
│   │   │   │   ├── OLED.c/h          ← 显示
│   │   │   │   ├── Timer_1ms.c       ← 定时器
│   │   │   │   └── hal_gpio.c        ← GPIO+中断
│   │   │   └── User/
│   │   │       ├── main.c            ← V3F主程序(96行)
│   │   │       └── ch32h417_it.c     ← V3F中断处理
│   │   ├── V5F/
│   │   │   ├── Hardware/
│   │   │   │   └── hardware.c        ← V5F硬件初始化(空)
│   │   │   └── User/
│   │   │       ├── main.c            ← V5F主程序(37行)
│   │   │       └── ch32h417_it.c     ← V5F HardFault诊断
│   │   └── python_csp/               ← CSP模型训练Python脚本
│   ├── CPU/IPC/IPC/                  ← 官方IPC示例(重要参考)
│   │   └── Common/hardware.c         ← 官方ping-pong示例
│   └── SRC/                          ← CH32H417 HAL/驱动库
│       ├── Peripheral/src/ch32h417_ipc.c  ← IPC底层驱动
│       └── Core/core_riscv.h               ← IPC_Type定义
├── CH32_waveform_client/             ← Android波形客户端(旧版Java)
├── ROADMAP.md                        ← 长期优化规划
└── PROJECT_CONTEXT.md                ← 本文件
```

---

## 三、IPC双核通信机制（核心）

### 3.1 硬件机制
- **通道**: IPC_CH0, TxCID1(V5F发送)/RxCID0(V3F接收)
- **Bit1**: V3F→V5F通知（V3F ENABLE Bit1，AutoEN设STS Bit1=1）
- **Bit0**: V5F→V3F ACK（V5F ENABLE Bit0，需手动SetFlagStatus确保STS=1）
- **ENA/STS/ISR**: 全局共享寄存器，ISR = ENA & STS（只读）
- **AutoEN**: 写ENA 0→1自动设STS，1→1不触发
- **MSG[0..3]**: 4个32位消息寄存器，双核共享

### 3.2 通信流程（已验证通过）
```
1. V3F: 写帧数据到共享slot → WriteMSG0(slot_addr) → ENABLE Bit1
2. V5F Handler: 检测Bit1 → 读MSG0 → 设pending_flag → DISABLE Bit1
3. V5F主循环: 处理帧(DSP/FFT/推理) → 写结果到slot → 设ack_ready=1
4. V5F Handler(下次通知时): 检测ack_ready → WriteMSG0(ack_data) → DISABLE Bit0 → ENABLE Bit0 → SetFlagStatus(Bit0)
5. V3F Handler: 检测Bit0 → 读MSG0 → 读slot结果 → DISABLE Bit0
```

### 3.3 已踩过的坑（重要！）
1. **中断风暴**: V5F从主循环ENABLE Bit0会触发自身CH0中断，Handler不清除Bit0导致无限重入。**修复**: ACK改为Handler内发送
2. **误杀ACK**: V3F Handler无条件DISABLE Bit0会清除V5F刚设置的ENA。**修复**: DISABLE移回if块内
3. **STS=0问题**: V5F Init清除STS Bit0，AutoEN在ENA已为1时不触发。**修复**: 移除ClearFlagStatus + DISABLE+ENABLE+SetFlagStatus三步
4. **ENA竞态**: read-modify-write非原子，双核并发写可能丢失更新（已知风险，暂未修）
5. **NVIC死锁**: 双方同时NVIC_DisableIRQ+busy-wait互相等待。**修复**: 不用NVIC屏蔽，改Handler内发ACK

---

## 四、当前系统状态

### 4.1 已验证通过的里程碑
| Git标签 | 内容 | 状态 |
|---|---|---|
| v3.1-fpu | V5F硬件FPU启用 | ✅ |
| v3.2-ipc-split | V5F中断/主循环分离 | ✅ |
| v3.3-dma-doublebuf | ADS1299 RX DMA DoubleBuffer | ✅ |
| v3.4-nvic-priority | 中断优先级框架修复 | ✅ |
| (最新commit) | IPC双核通信完全打通 | ✅ ack≈notify |

### 4.2 当前诊断数据（正常状态）
```
IPCDIAG,ack=5132,notify=5150,ok=1303,bad=3829,v5fhb=5150,ENA=00000000,STS=00000001,ISR=00000000
```
- ack≈notify: 双核通信正常
- v5fhb=notify: 1:1，无中断风暴
- ok/bad≈25%: parse验证通过率偏低，根因是共享slot无HSEM互斥保护，V5F ACK延迟1帧导致slot被覆盖

### 4.3 运行模式
系统支持3种模式（在Signal_Analysis.c中`EEG_APP_MODE`宏选择）：
- **COLLECT**: 数据采集+方向训练（REST→LEFT→RIGHT交替）
- **INFER**: 实时推理（使用训练好的模型）
- **CALIBRATE**: 校准模式

### 4.4 串口协议
- **DEBUG口**: 系统日志、IPCDIAG、DIRCSV特征数据、DIRCFG配置
- **WIFI口**: 预留ESP8266通信
- **命令格式**: `MODE,SET,n` / `MODE,TRAIN` / `MODE,TEST` / `TRIAL,LEFT` / `TRIAL,RIGHT`
- **数据格式**: `DIRCSV,label,ch0,...,ch7,feat0,...,feat23` / `RESULT,window=...,INTENT=...,S_LEFT=...,S_RIGHT=...,CONF=...,trained=...`

---

## 五、Android端现状

### 5.1 现有代码
`CH32_waveform_client/` 包含旧版Java Android客户端，功能：
- 蓝牙/USB串口连接
- 实时波形显示
- 基本训练流程

### 5.2 待改进
- 需要重构为Kotlin + Modern Android架构
- 需要支持WiFi(ESP8266)连接方式
- 需要与下位机串口协议对齐
- 需要支持训练流程可视化、模型参数下发
- 需要支持实时推理结果展示

---

## 六、协作规范

### 6.1 分支策略
```
master          ← 稳定发布
├── dev/firmware  ← 下位机开发分支
├── dev/android   ← Android开发分支
└── dev/shared    ← 共享协议/文档
```

### 6.2 职责划分
| 角色 | 负责范围 | 关键文件 |
|---|---|---|
| **下位机AI** | V3F/V5F固件、IPC通信、ADS1299驱动、信号处理算法 | `EXAM/GPIO/EEG/` |
| **Android AI** | Android客户端、串口协议对接、训练UI、波形显示 | `CH32_waveform_client/` 或新建目录 |
| **共享** | 串口通信协议、数据格式定义 | 待建立 `docs/protocol.md` |

### 6.3 接口契约
下位机与Android之间的接口必须明确定义：
1. **物理层**: USB串口 / Bluetooth / WiFi(ESP8266)
2. **数据链路层**: 波特率、帧格式、校验
3. **应用层**: 命令集、数据上报格式、训练流程控制

### 6.4 Git提交规范
```
<type>(<scope>): <subject>

type: feat/fix/refactor/docs/chore/test
scope: ipc/ads1299/android/protocol/oled/serial
```

---

## 七、待做事项（按优先级）

### Phase 1: 稳定性（下位机）
1. HSEM互斥保护共享帧槽（解决bad>ok）
2. ENA寄存器竞态修复（原子操作或软件锁）
3. 序号单调性检查 + CRC32校验
4. 24小时压力测试

### Phase 2: 架构重构（下位机）
1. Signal_Analysis.c拆分（2450行→6模块）
2. dualcore_ipc.c拆分（1297行→多模块）
3. 共享信号处理算法提取

### Phase 3: 功能增强
1. USB/ETH数据传输
2. Android应用重构
3. 训练流程优化

### Phase 4: 未来扩展
1. 在线学习/自适应模型
2. 多模态融合(EEG+IMU)
3. 低功耗优化

---

## 八、关键参考文件

| 文件 | 用途 |
|---|---|
| `EXAM/CPU/IPC/IPC/Common/hardware.c` | 官方IPC ping-pong示例，**必须严格对照** |
| `EXAM/SRC/Peripheral/src/ch32h417_ipc.c` | IPC底层驱动实现 |
| `EXAM/SRC/Core/core_riscv.h` | IPC_Type结构体定义(CTLR/ISR/ISM/ENA/STS/SET/CLR/MSG) |
| `EXAM/GPIO/EEG/Common/dualcore_ipc.c` | 当前IPC核心实现 |
| `EXAM/GPIO/EEG/V3F/Hardware/Signal_Analysis.c` | 采集主循环+推理 |
| `ROADMAP.md` | 长期优化规划 |

---

## 九、开发环境

- IDE: MounRiver Studio (WCH官方RISC-V IDE)
- 工具链: WCH RISC-V GCC
- 调试: WCH-Link SWD
- 串口: 115200bps, 8N1
- 编译: 无Makefile，通过MounRiver Studio .project配置