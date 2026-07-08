# EEG双核脑电采集与MI方向识别系统 — 项目上下文

> 版本：v3.0 | 日期：2026-07-08 | 仓库：https://github.com/Nihilistttttt/EEG.git

---

## 一、项目概述

基于CH32H417双核MCU（V3F+V5F）的EEG采集与运动想象(MI)方向识别系统：
- **V3F核**：ADS1299采集、专注度/放松度算法、OLED/串口/WiFi、姿态监护
- **V5F核**：Goertzel频谱、24维特征、SVM/CSP方向推理
- **ESP8266**：MCU↔Android纯透传桥梁（135行Arduino）
- **Android医生端**：训练控制+波形+推理+姿态监护
- **Android患者端**：被动接收训练指令+专注度显示

---

## 二、系统架构

```
MCU(V3F) ──[V1帧+文本行]──> ESP8266 ──[纯透传]──> Android(医生端/患者端)
Android  ──[文本命令]──> ESP8266 ──[纯透传(只拦截PING)]──> MCU
```

### 2.1 双核IPC

```
V3F (采集+专注度+外设主控)          V5F (信号处理+方向推理后端)
┌──────────────────────┐              ┌──────────────────────┐
│ ADS1299 SPI+DMA      │              │                      │
│ Ring Buffer           │───IPC CH0──→ │ Goertzel频谱         │
│ FFT 256pt + 专注度    │   Bit1通知   │ 24维特征提取          │
│ OLED/Serial/WiFi      │   Bit0 ACK← │ SVM/CSP方向分类器     │
│ ICM42605 姿态检测      │              │ 结果写共享slot        │
│ ACK重发机制            │              │                      │
└──────────────────────┘              └──────────────────────┘
```

### 2.2 串口协议

**命令(下行)**:
- `MODE,SET,<n>` — 1=COLLECT, 2=INFER, 3=COLLECT_CSP
- `MODE,TRAIN` / `MODE,TEST` — 训练/测试模式
- `TRIAL,LEFT` / `TRIAL,RIGHT` — 试次
- `STOP` — 停止并重置为IDLE
- `IPCDIAG,ON/OFF` / `POSTURE,ON/OFF` — 诊断开关
- `ACK,<seq>` — 确认收到关键消息

**数据(上行, 带seq的关键消息需ACK)**:
- `RESULT,seq=N,src=V5F,window=N,dt_ms=2000,win_rows=4,INTENT=LEFT|RIGHT,S_LEFT=N,S_RIGHT=N,CONF=N,trained=1`
- `TASK,DONE,seq=N,label=N,rows=20,skip=4`
- `POSTURE_STATE,seq=N,<姿态>,turns=N`
- `TURN_EVENT,seq=N,count=N,from=X,to=X,tick=N`
- `FALL_EVENT,seq=N,...` / `NO_TURN_ALERT,seq=N,duration_min=N`
- `TASK,<LEFT|RIGHT>,start` / `READY_TRAIN` / `READY_TEST` / `MODE_SET_OK,N`
- `MODE_DIAG,app=N,work=N,paused=N` — 无条件模式诊断
- `V5F_RAW,app=N,valid=N,cnt=N,fft=N,fv=N,pred=N` — 无条件V5F状态
- `V5F_DIAG,valid=N,cnt=N,last_cnt=N,trained=N,pred=N` — INFER分支内诊断

---

## 三、关键架构决策

| 决策 | 选择 | 理由 |
|------|------|------|
| V3F推理 | 已移除，统一V5F | 避免双核结果矛盾 |
| ESP8266角色 | 纯透传 | ESP8266单核，应用逻辑会抢占WiFi栈 |
| 上行协议 | V1帧+文本行 | 二进制帧高效，文本行易解析 |
| 下行协议 | 纯文本命令 | 简单可靠，ESP8266只拦截PING |
| ACK重发 | MCU端seq+超时重发 | 关键消息(RESULT/TASK,DONE/姿态)可靠投递 |
| CRC | 暂不加 | 先打通链路，后续按需添加 |
| 推理路径 | 只用V5F | V3F_LOCAL_INFER_ENABLE=0 |
| MCU输出 | 双串口(DEBUG+WIFI) | 所有输出必须同时走两个串口 |
| UNKNOWN处理 | 按分数强制选LEFT/RIGHT | 不输出UNKNOWN，压低不确定结果 |
| 姿态发送 | 仅状态变化时 | 减少ESP8266占用 |
| MI训练 | 4组(LEFT,RIGHT,LEFT,RIGHT) | 模型权重预编译，训练后直接可用 |

---

## 四、ACK重发机制

### 设计
- MCU关键消息带`seq=N`，存入重发缓冲区
- 主循环`Retry_Tick()`检查：超时300ms未收ACK则重发
- Android收到后回`ACK,N`
- MCU收到ACK后清空缓冲区
- RESULT重发最多7次，姿态事件最多20次

### 已修复的bug
- **seq不匹配**：`Retry_GetSeq()`返回递增前值，消息写`seq=N`但`Retry_Store()`递增后`s_retry_seq=N+1`，ACK回N时匹配不上。修复：`Retry_GetSeq()`改为返回`s_retry_seq+1`

### Android端去重
- `isDuplicateSeq(line)`基于seq号去重
- TASK,DONE和RESULT重复消息不会导致试次计数翻倍

---

## 五、已踩过的坑（必读）

### 5.1 双核IPC（已解决）

1. **中断风暴**：V5F从主循环ENABLE Bit0→触发自身CH0 Handler→无限重入。修复：ACK改为Handler内发送
2. **误杀ACK**：V3F Handler无条件DISABLE Bit0→清除V5F刚设的ENA。修复：DISABLE移回if块内
3. **STS=0问题**：V5F Init清STS Bit0 + AutoEN在ENA已为1时不触发。修复：移除ClearFlagStatus + 三步法
4. **NVIC死锁**：双方NVIC_DisableIRQ+busy-wait互相等待。修复：不用NVIC屏蔽
5. **ACK历史索引bug**：slot_idx=seq%64索引256项历史表，seq≥64时错位。修复：统一seq%256
6. **HSEM互斥**：尝试3次均卡死，已放弃，用DUALCORE_FENCE()

### 5.2 通信链路（已解决）

1. **ESP8266 WiFi栈饥饿**：loop()中`while(Serial.available())`持续占用CPU，WiFi栈得不到yield()。修复：TCP下行优先+批量读取+yield()
2. **Android OutputStream非线程安全**：UI线程和心跳线程同时写socket导致error=null。修复：BlockingQueue+专用senderThread
3. **handleClient竞争条件**：旧连接退出时`deviceClient=null`覆盖新连接。修复：`if(deviceClient==client)`才置null
4. **MCU背靠背发送合并**：两个Serial.printf被IDLE中断合并成一个包。修复：Parse_Command按`\n`分割逐行解析
5. **MCU响应只发DEBUG串口**：ESP8266连WIFI串口，Android收不到。修复：RESP宏双串口
6. **Android FrameParser丢文本**：V1帧二进制数据干扰文本行累积。修复：parseMcuTextResponses作为快速文本通道

### 5.3 推理链路（已解决）

1. **RESULT只发WIFI串口**：DEBUG串口看不到推理结果，误以为推理没工作。修复：双串口输出
2. **trained=0**：CSP4CH_WEIGHTS_TRAINED=0但代码直接用CSP的trained状态覆盖FFT分类器的。修复：CSP未训练时回退到DIR4CH_WEIGHTS_TRAINED
3. **V5F_DIAG没出现**：诊断在INFER分支内，如果g_eeg_app_mode!=2就看不到。修复：添加MODE_DIAG/V5F_RAW无条件诊断
4. **stopInference不发STOP**：只发MODE,SET,1导致MCU状态不一致。修复：改发STOP
5. **UNKNOWN太多**：V5F分类器返回pred=2时输出UNKNOWN。修复：按S_LEFT/S_RIGHT强制选方向

### 5.4 训练流程（已解决）

1. **训练5/4显示**：4组试次完成后startModelTraining()错误发MODE,TRAIN→READY_TRAIN→handleReadyTrain→enterRestPhase显示5/4。修复：4组完成后直接trainingComplete()
2. **TASK,DONE重复计数**：ACK重发导致Android多次收到TASK,DONE，试次翻倍。修复：seq去重
3. **TASK,DONE只发WIFI**：DEBUG串口看不到训练完成。修复：双串口

### 5.5 姿态监护（已解决）

1. **TURN_EVENT等只发WIFI**：DEBUG串口看不到。修复：双串口
2. **POSTURE_STATE仅变化时发送**：设备启动后第一次检测到姿态时发送。正常行为
3. **PostureFragment tvTurnCount映射错误**：和onTurnEvent共用View，翻身次数被事件文本覆盖。修复：翻身次数合并到tv_posture_state

---

## 六、当前状态

### 已完成
- 双核IPC通信完全打通（ack≈notify, ok=100%）
- V3F推理移除，INFER模式统一使用V5F推理结果
- ESP8266纯透传（135行），MCU↔Android全链路打通
- V5F推理链路验证：RESULT正常输出，trained=1
- 专注度/放松度算法全面重写（P0-P5, A1-A4）
- IIR系数/状态分离，FFT静态分配，logf查表优化
- ACK重发机制（seq+超时重发+去重）
- 姿态监护（ICM42605+姿态分类+翻身/坠床/久未翻身检测）
- Android医生端：训练/推理/姿态/专注度/波形页面
- Android患者端：专注度/放松度显示

### 待做
1. **ACK延迟高**：Android端ACK响应0.3-1.7秒，导致不必要的重发。需优化Android端处理速度
2. **推理全RIGHT**：S_LEFT极低(4-85)，S_RIGHT极高(9915-9996)，疑似分类器权重问题或特征提取问题
3. **SSVEP训练模块重写**：参考SSVEP.py，滤波/打分放下位机，闪烁/显示交Android
4. **姿态上方不更新**：PostureFragment绿色状态文本有时不跟随变化

---

## 七、关键文件速查

### MCU
| 文件 | 用途 |
|------|------|
| V3F/Hardware/Signal_Analysis.c | 主调度器(~315行)+姿态事件+Retry_Tick |
| V3F/EEG_FFT/eeg_fft.c | FFT状态机+推理输出+MODE_DIAG/V5F_RAW |
| V3F/EEG_Protocol/eeg_cmd_parser.c | 命令解析+ACK重发机制 |
| V3F/EEG_Direction/eeg_direction_collect.c | DIRCSV采集+TASK,DONE |
| V3F/Hardware/posture_detect.c | 姿态分类 |
| V3F/Hardware/patient_monitor.c | 翻身/坠床/久未翻身 |
| Common/dualcore_v5f_dsp.c | V5F DSP算法链+trained标志 |
| Common/dualcore_ipc_v3f.c | V3F读取V5F推理结果 |

### ESP8266
| 文件 | 用途 |
|------|------|
| CH32_Waveform/CH32_Waveform.ino | 纯透传135行 |

### Android医生端
| 文件 | 用途 |
|------|------|
| TcpServerManager.java | TCP服务+帧解析+文本解析+ACK发送+seq去重 |
| MiTrainingFragment.java | MI训练4组流程 |
| InferenceFragment.java | 推理显示(方向箭头) |
| PostureFragment.java | 姿态监护+翻身计数 |
| CommandSender.java | 命令发送封装 |
| DataDispatcher.java | 事件分发 |

---

## 八、Git规范

- 仓库: https://github.com/Nihilistttttt/EEG.git
- 推送: `git -c http.sslVerify=false push origin master`
- android分支拉取: `git fetch origin android && git checkout origin/android -- doctor patient`
- 不要擅自提交/推送
- 不要命令行编译CH32H417项目
