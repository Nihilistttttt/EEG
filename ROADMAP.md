# EEG 双核脑电采集与SSVEP训练系统 — 长期优化与功能实现规划

> 版本：v1.0 | 日期：2026-07-06 | 基线：v3.5-ipc-ch01（回退CH0单通道后）

---

## 一、项目现状评估

### 1.1 已完成里程碑

| 标签 | 内容 | 状态 |
|------|------|------|
| v3.1-fpu | V5F硬件FPU启用 + 废弃模块清除 | ✅ 已验证 |
| v3.2-ipc-split | V5F中断/主循环分离 + HardFault诊断 + IPC防御 | ✅ 已验证 |
| v3.3-dma-doublebuf | ADS1299 RX DMA DoubleBuffer + Circular | ✅ 已验证 |
| v3.4-nvic-priority | V3F/V5F中断优先级框架修复 | ✅ 已验证 |
| v3.5-ipc-ch01 | CH0/CH1通道分工尝试 → 回退 | ❌ 已回退 |

### 1.2 当前架构问题

| 编号 | 问题 | 严重度 | 影响 |
|------|------|--------|------|
| A1 | V3F/V5F信号处理链路完全重复（~800行） | P1 | 维护成本高，系数漂移风险 |
| A2 | 双核slot竞态条件（V3F清零与V5F读写无HSEM互斥） | P0 | 推理结果可能不可靠 |
| A3 | V3F在INFER模式下同时运行自身推理+V5F推理 | P1 | CPU浪费、输出混乱 |
| A4 | dualcore_ipc.c 1277行单文件，职责混杂 | P2 | 可读性差、编译慢 |
| A5 | Signal_Analysis.c 2437行单文件，职责混杂 | P2 | 同上 |
| A6 | IPC校验和失败无恢复机制 | P1 | 连续失败时V5F处理损坏数据 |
| A7 | DMA传输错误无检测 | P1 | 丢帧无感知 |
| A8 | FFT内存分配失败死锁while(1) | P0 | 设备永久挂起 |
| A9 | ESP8266/WiFi通信未集成 | P2 | 远程监控不可用 |
| A10 | Android双端代码几乎完全相同 | P2 | 维护成本高 |

### 1.3 资源使用概况

| 资源 | V3F | V5F | 空闲 |
|------|-----|-----|------|
| CPU | ~60%（采集+自身推理+IPC） | ~30%（DSP+推理） | V5F有大量余量 |
| RAM | ~46KB/448KB（10%） | ~6.4KB/256KB（2.5%） | 充足 |
| Flash | 64KB | 128KB | 充足 |
| DMA | 6/16通道 | 0/16通道 | DMA2完全空闲 |
| IPC | CH0单通道 | CH0单通道 | CH1~CH3空闲 |
| HSEM | ID0（启动同步） | ID0 | ID1~ID31空闲 |

---

## 二、核心功能清单（按优先级排序）

### P0 — 必须完成（系统稳定性）

| 编号 | 功能 | 说明 |
|------|------|------|
| F01 | **HSEM互斥保护共享帧槽** | 参考官方HSEM_DataSharing示例，用HSEM保护g_ipc_v3f_frame_slot的读写 |
| F02 | **消除FFT分配死锁** | 替换while(1)为错误恢复（复位DSP状态+跳过当前窗口） |
| F03 | **DMA传输错误检测** | 添加DMA_TEC（传输错误清除）中断回调，记录丢帧计数 |
| F04 | **V3F推理路径精简** | INFER模式下V3F仅做采集+IPC发送，不再运行自身FFT/推理 |

### P1 — 应该完成（架构质量）

| 编号 | 功能 | 说明 |
|------|------|------|
| F05 | **共享信号处理算法提取** | IIR/漂移/特征构建/分类器提取到Common/共享模块 |
| F06 | **dualcore_ipc.c模块拆分** | 拆分为ipc_transport.c（通信层）+ v5f_dsp.c（信号处理层）+ v5f_infer.c（推理层） |
| F07 | **Signal_Analysis.c模块拆分** | 拆分为eeg_capture.c + eeg_fft.c + eeg_direction.c + eeg_oled.c + eeg_cli.c |
| F08 | **IPC帧校验与恢复** | CRC16校验替代累加和 + 连续N次失败触发V5F DSP状态重置 |
| F09 | **V3F/V5F共享常量统一** | 采样率/FFT点数/通道数/滤波器系数统一到Common/eeg_common.h |
| F10 | **V5F DSP状态健康监控** | V3F定期检查V5F心跳（recv_count增长），超时则触发V5F复位 |

### P2 — 可以完成（功能增强）

| 编号 | 功能 | 说明 |
|------|------|------|
| F11 | **USB 3.0数据传输** | 参考USBSS CH372Device示例，实现EEG数据流高速传输到PC |
| F12 | **以太网远程监控** | 参考ETH TCP/MQTT示例，实现远程数据上报 |
| F13 | **ESP8266 WiFi通信集成** | 利用Serial.c中已有的SERIAL_PORT_WIFI，添加ESP8266 AT驱动 |
| F14 | **Android应用重构** | Server/Client分离，统一通信协议，Material Design 3 |
| F15 | **内部Flash模型参数A/B分区** | 支持在线更新推理模型权重，无需重新烧录固件 |
| F16 | **HDLC帧协议** | 串口通信增加SEQ+CRC16+ACK，保证数据完整性 |

### P3 — 未来扩展

| 编号 | 功能 | 说明 |
|------|------|------|
| F17 | **SerDes板间级联** | 多块EEG采集板通过SerDes高速串行互连 |
| F18 | **8通道EEG扩展** | 启用ADS1299全部8通道，特征维度从24扩展到48 |
| F19 | **实时SSVEP刺激与检测** | V5F运行SSVEP频率检测，V3F控制LED刺激 |
| F20 | **FreeRTOS迁移** | 参考官方FreeRTOS_Core示例，V3F/V5F各自运行RTOS |

---

## 三、总体优化方向分析

### 3.1 系统架构优化

| 优化项 | 适用场景 | 优点 | 缺点 | 预期收益 |
|--------|---------|------|------|----------|
| **共享RAM + HSEM替代纯IPC** | 双核数据传输 | 消除竞态、带宽高、延迟低 | 需修改Link脚本、增加临界区复杂度 | 可靠性↑↑、延迟↓90% |
| **V3F精简为纯采集核** | INFER模式 | CPU负载↓50%、代码量↓40% | V3F失去独立推理能力（V5F可覆盖） | 稳定性↑、维护成本↓ |
| **模块化拆分** | 代码维护 | 单文件<500行、职责清晰 | 需调整编译配置、头文件依赖 | 可读性↑↑、编译速度↑ |
| **共享算法库** | V3F/V5F代码复用 | 消除~800行重复代码 | 需仔细处理volatile/非volatile差异 | 代码量↓30%、一致性↑ |

**共享RAM + HSEM方案详细对比**：

| 方案 | 当前（纯IPC） | 目标（HSEM保护共享RAM） |
|------|-------------|----------------------|
| 数据传输 | IPC CH0中断 + MSG寄存器传地址 | 共享RAM直接访问 + HSEM互斥 |
| 互斥保护 | 无（仅DUALCORE_FENCE内存屏障） | HSEM_FastTake/ReleaseOneSem |
| 竞态风险 | 高（V3F清零slot时V5F可能正在读写） | 低（HSEM保证原子性） |
| 延迟 | ~10μs（中断响应+Handler处理） | ~0.1μs（HSEM取锁+直接内存访问） |
| 带宽 | 受限于IPC中断频率 | 仅受限于RAM访问速度 |
| 参考实现 | 无官方参考 | 官方HSEM_DataSharing示例 |

**Link脚本修改方案**（参考官方示例）：

```
V3F Link:
  RAM_SHARED (xrw) : ORIGIN = 0x20178000, LENGTH = 32K

V5F Link:
  RAM_SHARED (xrw) : ORIGIN = 0x20178000, LENGTH = 32K
  INPUT(../../V3F/obj/Common/ipc_shared.o)  // 链接V3F的共享数据目标文件
```

### 3.2 性能优化

| 优化项 | 适用场景 | 优点 | 缺点 | 预期收益 |
|--------|---------|------|------|----------|
| **V5F Goertzel替代FFT** | 已实施 | 计算量O(N×K) vs O(NlogN) | 仅需特定频率点时有效 | 已实现，无需改动 |
| **V5F I-Cache启用** | 代码执行加速 | 零成本加速 | 需确保共享RAM区域不被缓存 | 执行速度↑10~30% |
| **DMA2利用** | V5F侧数据搬运 | 解放CPU | 当前V5F无DMA需求 | 预留 |
| **硬件CRC加速帧校验** | IPC帧校验 | 4周期完成CRC32 | 需修改校验算法 | 校验速度↑100x |

### 3.3 代码质量优化

| 优化项 | 适用场景 | 优点 | 缺点 | 预期收益 |
|--------|---------|------|------|----------|
| **Slot字段操作去重** | dualcore_ipc.c 3处重复 | 消除~60行重复 | 需新增2个静态函数 | 可维护性↑ |
| **硬编码常量统一** | 两核参数一致性 | 消除参数漂移风险 | 需新建共享头文件 | 一致性↑↑ |
| **错误处理补全** | DMA/IPC/FFT异常 | 防止静默数据损坏 | 增加代码量 | 可靠性↑↑ |
| **volatile正确性审查** | 共享数据访问 | 防止编译器过度优化 | 需逐个审查 | 正确性↑ |

### 3.4 用户体验优化

| 优化项 | 适用场景 | 优点 | 缺点 | 预期收益 |
|--------|---------|------|------|----------|
| **USB 3.0数据传输** | PC端实时显示 | 带宽5Gbps，即插即用 | 需开发PC端驱动/应用 | 体验↑↑↑ |
| **Android应用重构** | 移动端监控 | 现代UI、协议统一 | 工作量大 | 体验↑↑ |
| **串口CLI增强** | 调试/配置 | 人机交互友好 | 增加代码量 | 调试效率↑ |
| **OLED状态面板** | 设备端显示 | 实时显示推理结果/系统状态 | OLED刷新占用CPU | 体验↑ |

---

## 四、具体实施路径与落地方案

### 4.1 Phase 1 — 稳定性修复（预计2~3天）

#### F01: HSEM互斥保护共享帧槽

**技术选型**：
- 方案A：仅对slot的V5F回写字段加HSEM保护（最小改动）
- 方案B：参考HSEM_DataSharing，将帧槽移入.shared_data section + HSEM保护（推荐）

**落地方案（方案B）**：

1. 修改V3F/V5F Link脚本，新增RAM_SHARED区域（0x20178000, 32K）
2. 新建`Common/ipc_shared.h` + `Common/ipc_shared.c`（V3F定义，V5F通过INPUT链接）
3. 将`g_ipc_v3f_frame_slot[]`声明为`__attribute__((section(".shared_data")))`
4. V3F写slot前`HSEM_FastTake(HSEM_ID1)`，写完`HSEM_ReleaseOneSem(HSEM_ID1, 0)`
5. V5F读/写slot前同样获取HSEM_ID1
6. 移除`DUALCORE_FENCE()`，由HSEM的硬件原子性保证内存可见性

**参考**：`EXAM/CPU/HSEM/HSEM_DataSharing/`

#### F02: 消除FFT分配死锁

**落地方案**：
- `FFT_Alloc()`失败时不再`while(1)`，改为返回NULL
- 调用方检查返回值，失败时调用`DualCore_V5F_ResetDSP()`重置状态，跳过当前窗口

#### F03: DMA传输错误检测

**落地方案**：
- 在ADS1299 DMA中断中添加`DMA1->INTFCR`错误标志检查
- 新增`g_ads1299_dma_error_count`计数器
- 错误时记录并继续（DMA Circular模式会自动恢复）

#### F04: V3F推理路径精简

**落地方案**：
- `EEG_APP_MODE_INFER`宏下，V3F主循环仅执行：ADS1299采集 → IPC发送 → ACK读取 → 串口输出V5F结果
- 移除V3F自身的`Process_FFT_Step()`、`Direction_InferAndPrint()`调用
- 保留V3F的FFT/推理代码（`#if DIR_INFER_1S_MODE`条件编译），供COLLECT模式使用

### 4.2 Phase 2 — 架构重构（预计5~7天）

#### F05: 共享信号处理算法提取

**技术选型对比**：

| 方案 | 优点 | 缺点 | 推荐 |
|------|------|------|------|
| A: Common/共享.c文件 + 条件编译 | 简单、无额外依赖 | volatile/非volatile需宏处理 | ✅ |
| B: 函数指针 + 注册模式 | 灵活、解耦 | 增加间接调用开销 | ❌ |
| C: 宏模板（#include .c） | 零开销 | 可读性差 | ❌ |

**落地方案（方案A）**：

新建`Common/eeg_signal.h` + `Common/eeg_signal.c`：

```
eeg_signal.h:
  - 统一数据类型：EEG_IIR_SOS_t, EEG_DriftRemove_t（替代DualCore_和V3F两套）
  - 统一常量宏：EEG_FS, EEG_FFT_SIZE, EEG_ACTIVE_CH_NUM, EEG_FEATURE_DIM
  - 统一滤波器系数：EEG_NOTCH_COEFF, EEG_BANDPASS_COEFF
  - 声明共享函数：EEG_IIR_SOS_Step(), EEG_RemoveRealtimeDrift(), 
                   EEG_BuildFeature(), EEG_SafePower(), EEG_LogRatio()

eeg_signal.c:
  - 实现上述所有函数
  - volatile通过wrapper宏处理：
    #if defined(Core_V5F)
      #define EEG_VOLATILE volatile
    #else
      #define EEG_VOLATILE
    #endif
```

#### F06: dualcore_ipc.c模块拆分

**目标文件结构**：

```
Common/
  ipc_transport.c/h    — IPC通道初始化、帧发送、ACK接收、中断处理（~400行）
  ipc_shared.c/h       — 共享帧槽定义（.shared_data section）（~100行）
  v5f_dsp.c/h          — V5F预处理流水线：漂移→陷波→带通→环形缓冲→FFT（~350行）
  v5f_infer.c/h        — V5F推理：SVM/CSP分类器 + 结果融合（~300行）
  v5f_mainloop.c/h     — V5F主循环入口 DualCore_V5F_MainLoopProcess()（~150行）
  eeg_signal.c/h       — 共享信号处理算法（~200行）
  eeg_common.h         — 统一常量、类型定义（~80行）
```

#### F07: Signal_Analysis.c模块拆分

**目标文件结构**：

```
V3F/Hardware/
  eeg_capture.c/h      — ADS1299采集主循环 + DMA回调（~400行）
  eeg_fft.c/h          — V3F FFT处理 + 频段积分（~350行）
  eeg_direction.c/h    — V3F方向特征 + 推理输出（~500行）
  eeg_oled.c/h         — OLED波形/状态显示（~200行）
  eeg_cli.c/h          — 串口命令解析 + 数据输出（~300行）
  eeg_wifi.c/h         — ESP8266 WiFi通信（预留，~100行）
```

#### F08: IPC帧校验与恢复

**技术选型**：

| 方案 | 校验强度 | 计算开销 | 推荐 |
|------|---------|---------|------|
| 累加和（当前） | 弱 | 极低 | ❌ |
| CRC16-CCITT | 中 | 低（查表法4周期/字节） | ✅ |
| 硬件CRC32 | 强 | 极低（4周期/整帧） | 未来可选 |

**落地方案**：
- 替换`DualCore_IPC_Checksum16()`为CRC16-CCITT查表法
- V3F连续3次校验失败时，通过IPC通知V5F执行`DualCore_V5F_ResetDSP()`

#### F09: V3F/V5F共享常量统一

**落地方案**：
- 新建`Common/eeg_common.h`，统一所有重复常量
- 逐步替换Signal_Analysis.c和dualcore_ipc.c中的本地宏定义

#### F10: V5F DSP状态健康监控

**落地方案**：
- V3F每100ms检查一次`g_ipc_v3f_ack_count`增长
- 若连续500ms无增长，通过IPC_MSG2写入0xDEAD0001（非致命复位标记）
- V5F主循环检查MSG2，若为0xDEAD0001则执行`DualCore_V5F_ResetDSP()`

### 4.3 Phase 3 — 功能增强（预计7~14天）

#### F11: USB 3.0数据传输

**技术选型**：

| 方案 | 带宽 | 复杂度 | 推荐 |
|------|------|--------|------|
| USB CDC虚拟串口 | ~12Mbps | 低 | 开发调试阶段 |
| USBHS CH372Device | ~480Mbps | 中 | 生产阶段（USB 2.0） |
| USBSS CH372Device | ~5Gbps | 高 | 未来扩展（USB 3.0） |

**落地方案（USBHS CH372Device优先）**：
1. 参考EXAM/USBHS/DEVICE/CH372Device/实现自定义USB设备
2. V3F作为USB设备端，PC端开发Python/C#接收应用
3. Bulk IN端点传输EEG数据帧（27字节/帧 × 250fps = 6.75KB/s）
4. Bulk OUT端点接收PC端命令（模式切换、参数配置）

**参考**：`EXAM/USBHS/DEVICE/CH372Device/`

#### F12: 以太网远程监控

**落地方案**：
1. 参考EXAM/ETH/TCPClient/，V3F作为TCP Client连接PC端TCP Server
2. 利用WCHNET协议栈（预编译库libwchnet.a）
3. 传输格式：与串口DIRCSV兼容的文本协议
4. 未来可扩展MQTT上报到云端

**参考**：`EXAM/ETH/TCPClient/`、`EXAM/ETH/MQTT/`

#### F13: ESP8266 WiFi通信集成

**落地方案**：
1. 利用Serial.c中已有的SERIAL_PORT_WIFI（USART2）
2. 新建`V3F/Hardware/eeg_wifi.c`，实现ESP8266 AT指令驱动
3. 支持STA模式连接路由器 + TCP/UDP数据传输
4. 双缓冲发送：DMA发送缓冲A时填充缓冲B

#### F14: Android应用重构

**落地方案**：
1. 统一包名为`com.eeg.ssveptrainer`
2. Server端（设备端）：TCP Server + 数据广播
3. Client端（控制端）：TCP Client + 实时显示
4. 升级到Material Design 3 + Kotlin + Jetpack Compose
5. 通信协议：二进制帧格式（帧头+SEQ+PAYLOAD+CRC16）

#### F15: 内部Flash模型参数A/B分区

**落地方案**：
1. V3F Flash末尾划分2个4KB分区（A区+B区）
2. 当前活动分区由header中的magic+version标识
3. PC端通过USB/串口发送新权重 → V3F写入非活动分区 → 校验 → 切换活动标记
4. 启动时读取活动分区的权重到RAM

#### F16: HDLC帧协议

**落地方案**：
1. 串口通信增加HDLC-like帧格式：0x7E帧头 + SEQ(1B) + CMD(1B) + PAYLOAD + CRC16 + 0x7E帧尾
2. 支持ACK/NACK重传
3. 逐字节转义（0x7E → 0x7D 0x5E）

### 4.4 Phase 4 — 未来扩展（按需）

#### F17~F20: 略（见核心功能清单）

---

## 五、分阶段执行时间表与里程碑

### Phase 1: 稳定性修复（v3.6-stable）

| 步骤 | 任务 | 预计耗时 | 交付物 |
|------|------|---------|--------|
| 1.1 | F02: 消除FFT分配死锁 | 0.5天 | DualCore_V5F_ResetDSP()错误恢复路径 |
| 1.2 | F03: DMA传输错误检测 | 0.5天 | g_ads1299_dma_error_count + 错误回调 |
| 1.3 | F04: V3F推理路径精简 | 1天 | INFER模式下V3F仅采集+IPC |
| 1.4 | F01: HSEM互斥保护 | 1.5天 | Link脚本修改 + HSEM保护slot读写 |
| 1.5 | 集成测试 + 回归验证 | 0.5天 | git tag v3.6-stable |

**里程碑**：v3.6-stable — 系统稳定性达标，双核通信可靠

### Phase 2: 架构重构（v3.7-refactor）

| 步骤 | 任务 | 预计耗时 | 交付物 |
|------|------|---------|--------|
| 2.1 | F09: 共享常量统一 | 1天 | Common/eeg_common.h |
| 2.2 | F05: 共享算法提取 | 2天 | Common/eeg_signal.c/h |
| 2.3 | F06: dualcore_ipc.c拆分 | 2天 | 6个新模块文件 |
| 2.4 | F07: Signal_Analysis.c拆分 | 2天 | 6个新模块文件 |
| 2.5 | F08: CRC16帧校验 | 1天 | CRC16查表 + 失败恢复 |
| 2.6 | F10: V5F健康监控 | 0.5天 | 心跳超时检测 + 自动复位 |
| 2.7 | 集成测试 + 回归验证 | 0.5天 | git tag v3.7-refactor |

**里程碑**：v3.7-refactor — 代码架构清晰，单文件<500行，V3F/V5F算法统一

### Phase 3: 功能增强（v3.8-usb → v3.9-network）

| 步骤 | 任务 | 预计耗时 | 交付物 |
|------|------|---------|--------|
| 3.1 | F11: USB 2.0 HS数据传输 | 3天 | USBHS CH372Device驱动 + PC端接收工具 |
| 3.2 | F15: Flash模型A/B分区 | 2天 | 权重在线更新机制 |
| 3.3 | F16: HDLC帧协议 | 1天 | 串口可靠传输 |
| 3.4 | 集成测试 | 0.5天 | git tag v3.8-usb |
| 3.5 | F13: ESP8266 WiFi集成 | 2天 | AT驱动 + TCP/UDP传输 |
| 3.6 | F12: 以太网远程监控 | 2天 | WCHNET TCP Client |
| 3.7 | F14: Android应用重构 | 5天 | 新包名 + Material 3 + 统一协议 |
| 3.8 | 集成测试 | 0.5天 | git tag v3.9-network |

**里程碑**：v3.8-usb — USB高速数据传输可用；v3.9-network — 多通道数据传输可用

### Phase 4: 未来扩展（v4.0+）

| 步骤 | 任务 | 预计耗时 | 交付物 |
|------|------|---------|--------|
| 4.1 | F17: SerDes板间级联 | 按需 | 多设备高速互连 |
| 4.2 | F18: 8通道EEG扩展 | 按需 | 48维特征 + 更高分辨率 |
| 4.3 | F19: SSVEP刺激与检测 | 按需 | 实时SSVEP闭环 |
| 4.4 | F20: FreeRTOS迁移 | 按需 | RTOS任务调度 |

**里程碑**：v4.0-multich — 多通道/多设备EEG系统

---

## 六、关键决策记录

| 决策 | 选项 | 结论 | 理由 |
|------|------|------|------|
| IPC通道方案 | CH0单通道 vs CH0/CH1分用 | CH0单通道 | 分用收益不足以抵消复杂度，CH0 Bit0/Bit1已能区分方向 |
| TX DMA DoubleBuffer | 实施vs不实施 | 不实施 | ADS1299 SPI TX为固定RDATA命令，无需双缓冲 |
| 共享RAM方案 | 纯IPC vs HSEM保护共享RAM | HSEM保护共享RAM | 消除竞态、降低延迟、参考官方示例 |
| FFT方法 | 复数FFT vs Goertzel | Goertzel（V5F）/ 复数FFT（V3F） | V5F仅需特定频率点，Goertzel更高效；V3F保留FFT供全频谱分析 |
| USB传输方案 | CDC vs CH372Device | CH372Device（生产）/ CDC（调试） | Bulk传输带宽远超CDC，但CDC开发更简单 |
| 信号处理代码复用 | 条件编译 vs 函数指针 | 条件编译 | 零运行时开销，嵌入式友好 |

---

## 七、风险与缓解

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|----------|
| HSEM共享RAM引入新竞态 | 中 | 高 | 严格参考官方HSEM_DataSharing示例，逐步迁移 |
| V3F精简后失去独立推理能力 | 低 | 中 | 保留条件编译开关，可随时切回 |
| USB驱动开发周期超预期 | 中 | 中 | 先用CDC模式验证数据通路，再迁移到CH372Device |
| 模块拆分引入编译/链接错误 | 中 | 低 | 每拆一个模块立即编译验证，小步提交 |
| Android重构工作量大 | 高 | 中 | 分阶段：先统一协议，再升级UI，最后迁移Kotlin |