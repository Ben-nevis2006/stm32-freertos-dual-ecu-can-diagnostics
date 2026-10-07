# 硬件到货阶段交接与基线汇总报告

## 1. 记录信息

| 项目 | 内容 |
|---|---|
| 项目名称 | 基于 STM32 + FreeRTOS 的双 ECU CAN 实时通信与故障检测系统 |
| 记录日期 | 2026-09-17 |
| 阶段变化 | 核心硬件已到货，项目由硬件到货前的软件基线阶段转入实物 Bring-up 阶段 |
| 当前 Git 基线 | `4377c5e feat: add dual-node FreeRTOS IPC baseline` |
| 仓库状态 | `main` 与 `origin/main` 同步，工作区在生成本报告前 clean |
| 报告用途 | 为上板、单外设验证、双节点 CAN、RTOS 运行验证和后续故障闭环提供统一前情提要 |

## 2. 当前结论

硬件到货前计划的软件准备工作已经形成可回退基线：开发环境、两个正式 CubeMX/CubeIDE 工程、双节点外设配置、FreeRTOS 任务骨架和 IPC 对象均已生成，并分别达到 `0 errors, 0 warnings`。当前结果证明配置、代码生成、编译和链接链路可用。

当前代码仍是软件骨架，不代表系统功能已经实现。任务函数仍为 `osDelay(1)` 占位循环，CAN 接收队列和状态邮箱尚未被业务代码使用，CAN 过滤器与启动流程、协议编解码、传感器采集、控制算法、Fault Manager、Fallback、Recovery 和诊断历史均未实现。硬件到货也不改变这条验证边界。

下一阶段应从硬件清点和两块 NUCLEO 的独立上电、下载、调试开始，不直接连接完整 12 V 功率路径，不直接进入整车式故障场景，也不同时调试 CAN、风扇、传感器和 RTOS。

## 3. 受控设计输入和变更原则

当前实现继续服从以下文档，优先级从上到下排列：

1. [System Architecture Definition / 系统架构定义 v0.1](../02_architecture_系统架构/System_Architecture_Definition_系统架构定义_v0.1_Synced_已同步_2026-09-02.docx)
2. [Fault List / 故障清单 v0.1](../02_architecture_系统架构/Fault_List_故障清单_v0.1_Synced_已同步_2026-09-02.docx)
3. [CAN Matrix / CAN 矩阵 v0.1](../04_can_communication_CAN通信设计/CAN_Matrix_CAN矩阵_v0.1_Synced_已同步_2026-09-02.xlsx)
4. [FreeRTOS Task Architecture / FreeRTOS 任务架构 v0.1](../02_architecture_系统架构/FreeRTOS_Task_Architecture_FreeRTOS任务架构_v0.1_Synced_已同步_2026-09-02.docx)
5. [Hardware Selection / 硬件选型 v0.1](Hardware_Selection_硬件选型_v0.1_Working_Baseline_工作基线_2026-09-08.docx)
6. [Architecture Synchronization Record / 架构同步记录](../01_governance_项目治理/Architecture_Synchronization_Record_架构同步记录_2026-09-03.docx)
7. [Peripheral, Pin, Clock & Interrupt Allocation / 外设、引脚、时钟与中断分配](Peripheral_Pin_Clock_Interrupt_Allocation_外设引脚时钟中断分配_v0.1_Working_Draft_工作草案_2026-09-13.md)

旧版 [`Project Charter / 项目章程（历史）`](../../archive/project%20charter.docx) 只保留立项背景作用。其中独立 Heartbeat、多条 10 ms 周期报文、每个功能单独建立 Task 等早期构想，已被上述同步文档替代。

项目内容已经冻结。降低对示波器、CAN 分析仪、电子负载和可调电源的依赖，不等于删减 Fault List、Fallback、Recovery、Mode 或多故障依赖关系。后续如需修改 CAN ID、DLC、Signal semantics、Fault scope、Task architecture 或监督边界，必须单独记录理由和影响，不能在 Bring-up 中临时改写基线。

## 4. 项目定位和系统边界

本项目面向苏州汽车电子和嵌入式软件实习岗位，优先展示 MCU、C、FreeRTOS、CAN、Sensor/Actuator、Diagnostics、Fault Injection、测试和工程追溯能力。项目不宣称实现汽车级 ECU 电源、EMC、瞬态防护、功能安全认证或完整 UDS 栈，也不把普通 PC 测试工具包装成 HIL。

系统职责保持如下：

- Node B 是 Supervisory ECU，拥有 Plant Temperature、热管理策略、Cooling Demand、系统级 Fault 监督和 System Mode 管理责任。
- Node A 是 Thermal Actuator ECU，拥有风扇 PWM、RPM、Current/Voltage、本地闭环、本地执行器 Fault 和无需等待 CAN 的必要保护责任。
- Node B 回答“系统需要多少冷却”，Node A 回答“执行器是否把冷却请求可靠执行”。
- System Mode 为 `INIT / NORMAL / DEGRADED / SAFE`。Fault 是诊断状态，不是独立运行模式；Recovery 是转移过程，不是稳定模式。
- 最终模式由全部 Active Confirmed Fault、Functional Availability、Observability 和 Protection State 综合重算，不按 Fault 数量或单一优先级机械决定。

## 5. 软件环境基线

| 组件 | 当前基线 | 状态 |
|---|---:|---|
| STM32CubeMX | 6.18.1 | 已验证 |
| CubeMX Device Database | DB 6.0.181 | 已验证 |
| STM32CubeF1 | 1.8.7 | 固定版本，已验证 |
| STM32CubeIDE | 2.2.0 | 已安装并完成双工程构建 |
| STM32CubeProgrammer | 2.19.0 | 已安装，硬件连接仍待验证 |
| GNU Tools for STM32 | GCC 14.3.1 | 已通过实际编译、链接和后处理验证 |
| CMSIS-RTOS API | CMSIS-RTOS v2 | 两个节点一致 |
| FreeRTOS | CubeF1 1.8.7 随附 FreeRTOS 10.3.1 | 两个节点一致 |

环境冒烟工程 `F103RB_SmokeTest` 达到 `0 errors, 0 warnings`，但它是独立、可丢弃的工具链验证工程，不属于正式 Node A/Node B 固件。

## 6. 硬件选型基线和到货状态

用户已确认硬件到货。该信息只表示项目可以进入实物阶段，具体数量、SKU、模块版本、外观、极性、跳线状态和电气连接尚需逐件验收。

| ID | 设备或材料 | 基线选择 | Bring-up 关注点 |
|---|---|---|---|
| HS-01 | MCU 开发板 | NUCLEO-F103RB ×2 | 分别确认 USB 枚举、ST-LINK、目标 MCU、下载和调试 |
| HS-02 | CAN 物理层 | Adafruit CAN Pal 5708 / TJA1051T/3 ×2 | 核对逻辑电源、CANH/CANL、终端电阻和模块丝印 |
| HS-03 | 风扇 | ARCTIC P8 PWM PST non-CO，12 V 4-wire | 不使用 PST 串联；确认线序、PWM 电平和 Tach 行为 |
| HS-04 | 温度采集 | 10 kΩ B3950 NTC + 10 kΩ ±1% 分压 | 先核对阻值，再接 Node B 3.3 V ADC |
| HS-05/06 | 电流与电压 | INA260 breakout ×1 | 3.3 V I2C；高边测风扇电流和 12 V bus voltage |
| HS-07 | Thermal Plant | 100 × 80 × 20 mm 散热器 + 10 Ω/25 W 加热电阻 | 12 V 下名义约 14.4 W；后续记录温升/降温曲线 |
| HS-08 | 电源 | 12 V/3 A 稳压适配器，5.5 × 2.1 mm | 核对极性；开发板初期仍使用 USB 供电；所有节点公共地 |

辅助基线包括 4.7 kΩ Tach 上拉、电阻与杜邦线、DC 转螺丝端子、KF128 接线端子、18 AWG 功率线、PWM 风扇延长线和 830 孔面包板。P8 原装线不直接剪，优先改延长线。

## 7. MCU 外设、引脚和时钟基线

### 7.1 两节点公共配置

| 功能 | 配置 |
|---|---|
| MCU / Board | STM32F103RBT6 / NUCLEO-F103RB |
| Debug | SWD，PA13/PA14，板载 ST-LINK |
| System Clock | HSI 8 MHz → HSI/2 → PLL ×16 → SYSCLK/HCLK 64 MHz |
| Bus Clock | PCLK1 32 MHz，APB1 Timer 64 MHz，PCLK2 64 MHz |
| CAN1 | PA11 RX、PA12 TX，500 kbit/s，Prescaler 4、BS1 13 TQ、BS2 2 TQ、SJW 1 TQ |
| CAN Sample Point | 87.5%（由位时序计算得出） |
| USART2 | PA2 TX、PA3 RX，115200 bit/s，8-N-1，无硬件流控 |
| Board LED | PA5 / LD2，粗粒度状态提示 |
| Board Button | PC13 / B1，开发测试入口，不参与正常控制 |

两块开发板必须使用完全相同的 CAN 位时序。当前采用 64 MHz HSI 路径，先以短线、室温台架的实际通信结果决定是否需要调整；在没有证据前不增加 HSE 改造。

### 7.2 Node A

| 功能 | 当前配置 |
|---|---|
| Fan PWM | TIM3_CH2 / PC7，Full Remap，AF Open-Drain，PSC 0，ARR 2559，配置频率 25 kHz |
| Fan Tach | TIM2_CH1 / PA0，下降沿输入捕获，PSC 639，ARR 65535，计数频率 100 kHz |
| INA260 I2C | I2C1 Remap，PB8 SCL、PB9 SDA，100 kHz |
| INA260 ALERT | PB5 入口保留，当前尚未启用 |

### 7.3 Node B

| 功能 | 当前配置 |
|---|---|
| NTC ADC | ADC1_IN0 / PA0，Rank 1，右对齐，软件触发 |
| ADC Mode | Continuous 关闭，Scan 关闭，不使用 DMA 和 ADC 中断 |
| Sampling Time | 55.5 cycles |
| ADC Clock | PCLK2/6，约 10.67 MHz，低于 STM32F103 14 MHz 上限 |

### 7.4 当前中断优先级

| 中断 | Node A | Node B | 约束 |
|---|---:|---:|---|
| CAN1 RX FIFO0 | 5 | 5 | ISR 只取帧并投递 Queue，不解析业务 |
| TIM2 Input Capture | 6 | - | 只保存捕获证据，不在 ISR 中做 Fault 判定 |
| B1 EXTI15_10 | 7 | 7 | 仅作为测试入口，不能承载正常控制逻辑 |
| TIM4 HAL Tick | 15 | 15 | HAL 1 ms timebase |
| SysTick / PendSV | 15 | 15 | FreeRTOS 内核调度 |

若后续启用 INA260 ALERT，计划优先级为 4，并且 ISR 不调用 FreeRTOS API；具体最小保护动作必须在驱动和保护设计中明确。

## 8. FreeRTOS 任务和内核基线

### 8.1 Node A application tasks

| Task | Trigger / Period | CMSIS Priority | Stack | 冻结职责 |
|---|---|---:|---:|---|
| `A_ControlTask` | 10 ms periodic | High，40 | 256 words / 1024 B | 读取最新 valid demand 和本地反馈，执行本地风扇控制与 Node A Fault 更新 |
| `A_ComRxTask` | CAN RX event 或监督 deadline | AboveNormal，32 | 256 words / 1024 B | 解析 0x100，维护 sequence、valid-command freshness 和 FA-07 |
| `A_Cyclic100msTask` | 100 ms periodic | Low，8 | 256 words / 1024 B | 读取最新状态快照并提交 0x180，维护 Status Alive |

严重本地保护属于 non-task Immediate Protection Path，不额外创建 Task。

### 8.2 Node B application tasks

| Task | Trigger / Period | CMSIS Priority | Stack | 冻结职责 |
|---|---|---:|---:|---|
| `B_ComRxTask` | CAN RX event 或监督 deadline | High，40 | 256 words / 1024 B | 解析 0x180，区分 raw reception 与 trusted freshness，维护 FB-03/FB-04 |
| `B_Cyclic100msSupervisorTask` | 100 ms periodic | Normal，24 | 256 words / 1024 B | 温度策略、Cooling Demand、系统 Fault/Mode 更新和 0x100 提交 |

### 8.3 内核配置

| 参数 | 两节点当前值 | 说明 |
|---|---:|---|
| Tick rate | 1000 Hz | 1 tick = 1 ms |
| Tick width | 32 bit | `configUSE_16_BIT_TICKS = 0` |
| `configMAX_PRIORITIES` | 56 | CMSIS-RTOS v2 适配层生成值，不需要人为改成 7 |
| FreeRTOS heap | `heap_4`，8192 B | 静态预留在 `.bss`，任务和 Queue 在运行时从中分配 |
| Stack overflow check | 2 | Hook 已生成但函数体尚未实现安全处理 |
| Malloc failed hook | Enabled | Hook 已生成但函数体尚未实现安全处理 |
| Software timers | Enabled | Timer task priority 2、queue length 10、stack 256 words |
| Newlib reentrant | Disabled | 当前不允许把并发任务中的重型 printf 当作默认设计 |
| Mutex objects | 0 | 内核能力虽启用，但应用基线不创建 Mutex；依靠 Owner + message passing |

当前优先级和栈是实现起点，不是最终性能结论。硬件运行后必须用 task period、response time、jitter、stack high-water mark 和 free/minimum-ever-free heap 证据进行收口。

## 9. IPC 和数据所有权基线

| IPC Object | 类型 / 深度 | Producer → Consumer | 语义 |
|---|---|---|---|
| `qA_CanRx` | FIFO Queue / 4 × `AppCanRxFrame_t` | CAN RX ISR → `A_ComRxTask` | 逐帧保留 0x100 事件，支持 Alive/sequence 检查 |
| `mbA_CommandState` | 1-slot Mailbox / `ACommandState_t` | `A_ComRxTask` → `A_ControlTask` | 最新可信 Cooling Demand、Alive、有效时间和 FA-07 状态 |
| `mbA_StatusSnapshot` | 1-slot Mailbox / `AStatusSnapshot_t` | `A_ControlTask` → `A_Cyclic100msTask` | 最新执行器状态；旧 snapshot 可覆盖 |
| `qB_CanRx` | FIFO Queue / 4 × `AppCanRxFrame_t` | CAN RX ISR → `B_ComRxTask` | 逐帧保留 0x180 事件 |
| `mbB_NodeAStatus` | 1-slot Mailbox / `BNodeAStatus_t` | `B_ComRxTask` → `B_Cyclic100msSupervisorTask` | 最新可信 Node A 状态及通信监督结果 |

`AppCanRxFrame_t` 保存接收 tick、11-bit Standard ID、DLC 和 8-byte data。Node B mailbox 同时保存 `last_raw_rx_tick` 与 `last_trusted_rx_tick`，因为 FB-03 和 FB-04 的时间语义不能合并。

Queue 和 Mailbox 使用动态创建。最新 `bss` 增量主要是 Queue Handle；实际控制块和缓冲区在启动时从已预留的 8192 B FreeRTOS heap 中取得。当前编译成功不等于运行时创建一定成功，必须在上板阶段检查 Handle、Malloc Failed Hook 和剩余 heap。

Mailbox 只表示当前状态，不承担 DTC/event history。运行状态恢复或 Fault bit 清除不能静默删除历史诊断事件。

## 10. CAN Matrix 基线

### 10.1 报文

| Message | ID | Direction | DLC | Cycle | Supervision |
|---|---:|---|---:|---:|---|
| `Cooling_Command` | `0x100` | Node B → Node A | 2 | 100 ms | 300 ms valid-command timeout |
| `Actuator_Status` | `0x180` | Node A → Node B | 5 | 100 ms | 300 ms raw timeout + 300 ms trusted freshness |

不增加独立 Heartbeat。两条周期报文本身承担节点活动和新鲜度监督。`0x100` 数值低于 `0x180`，总线竞争时控制命令具有更高仲裁优先级。

### 10.2 Signal packing

| Message | Byte / Bit | Signal | 编码要求 |
|---|---|---|---|
| 0x100 | Byte0 | Cooling Demand | 0–100 = 0–100%；101–254 Reserved；0xFF Invalid；0 明确表示 Fan OFF |
| 0x100 | Byte1.Bit0–3 | Command Alive | 0–15 循环，15→0 |
| 0x100 | Byte1.Bit4–7 | Reserved | Tx 固定为 0 |
| 0x180 | Byte0.Bit0–1 | Availability | 0 FULL、1 REDUCED、2 LOST、3 Reserved |
| 0x180 | Byte0.Bit2–3 | Control Mode | 0 IDLE_OFF、1 CLOSED_LOOP、2 OPEN_LOOP、3 SAFE_OUTPUT |
| 0x180 | Byte0.Bit4 | Fallback Active | 0/1 |
| 0x180 | Byte0.Bit5 | RPM Valid | 0/1 |
| 0x180 | Byte0.Bit6–7 | Reserved | Tx 固定为 0 |
| 0x180 | Byte1 | FA status | FA-01 bit0；FA-02 severity bits1–2；FA-03..FA-07 bits3–7 |
| 0x180 | Byte2.Bit0–3 | Status Alive | 0–15 循环，15→0 |
| 0x180 | Byte2.Bit4–7 | Reserved | Tx 固定为 0 |
| 0x180 | Byte3–4 | Actual RPM | Little-endian，1 rpm/bit，0–65534；0xFFFF Invalid |

### 10.3 监督和一致性规则

- 第一帧合法报文以其当前 Alive 建立同步，不要求从 0 开始。
- 单次 Alive freeze/jump/sequence anomaly 记为 suspected，并以当前计数器重新同步；下一帧正常递增则清除异常计数。
- 连续 2 次 sequence anomaly 才 Confirm FA-07 或 FB-04。
- 通信异常后连续 3 帧 valid/trusted 才完成 recovery re-qualification，并重新评估全部 Active Fault。
- Node A 在两次 100 ms valid command 之间对最新 Cooling Demand 进行 Sample-and-Hold。10 ms 控制循环不会每次“消费一条新命令”。
- 300 ms 无新 valid 0x100 后，FA-07 Confirmed，Hold Last 结束并进入 Local Conservative Cooling Demand。
- Node B 300 ms 完全收不到 0x180 时确认 FB-03 并进入 SAFE。
- 0x180 仍到达但 300 ms 没有可信状态时，FB-04 升级到 SAFE。
- `RPM_Valid = 0` 时 `Actual_RPM` 必须为 `0xFFFF`。
- `CLOSED_LOOP` 要求 RPM Valid；`OPEN_LOOP` 要求 Fallback Active；`SAFE_OUTPUT` 是本地保护状态且 Fallback Active 为 0。
- 正常 100 ms Tx 必须在每个应用周期窗口内提交；300 ms 是异常容忍边界，不是任务正常 deadline。

## 11. Fault List 完整基线

### 11.1 Node A local faults

| ID | Fault | 主要模式影响 | 当前实现状态 |
|---|---|---|---|
| FA-01 | Fan Stall | SAFE | 未实现 |
| FA-02 | Fan Speed Deviation | Level 1 DEGRADED；Level 2 SAFE | 未实现 |
| FA-03 | RPM Feedback Fault | Fallback eligible 时 DEGRADED，否则 SAFE | 未实现 |
| FA-04 | Motor Overcurrent | Severe 时本地立即保护并进入 SAFE | 未实现 |
| FA-05 | Current Feedback Fault | DEGRADED；禁用依赖电流的诊断 | 未实现 |
| FA-06 | Supply Voltage Fault | DEGRADED 或 SAFE | 未实现 |
| FA-07 | Cooling Demand Communication Fault | Last Valid → Local Conservative Demand；DEGRADED/SAFE | 未实现 |

### 11.2 Node B system faults

| ID | Fault | 主要模式影响 | 当前实现状态 |
|---|---|---|---|
| FB-01 | Overtemperature | SAFE | 未实现 |
| FB-02 | Insufficient Cooling Performance | DEGRADED，可发展到 SAFE | 未实现 |
| FB-03 | Node A Communication Timeout | SAFE | 未实现 |
| FB-04 | Node A Alive / Sequence Fault | DEGRADED，可升级 SAFE | 未实现 |
| FB-06 | Plant Temperature Signal Fault | Observability loss，SAFE | 未实现 |

`FB-05` 只保留历史编号和 Fault Propagation/Aggregation 语义，不实现为独立 Fault。Node A 应上报 underlying FA、信号有效性、Control/Fallback Mode 和 Availability，Node B 据此进行系统级聚合。

关键依赖规则继续保留：FA-06 可解释速度下降时允许抑制 FA-02 独立 Qualification；FA-03 Active 时 FA-01/FA-02 的可用性受限；FA-05 Active 时软件过流诊断和基于电流的 Stall discrimination 不可用；FB-06 Active 时 FB-01/FB-02 因温度无效而不可用。Fault 样本消失不等于恢复，运行模式恢复也不等于清除诊断历史。

## 12. 构建和内存基线

STM32F103RB 资源基准为 128 KiB Flash 和 20 KiB SRAM。Flash 按 `text + data` 估算，静态 RAM 按 `data + bss` 估算；`dec` 不能解释成一种物理存储器占用。

| 阶段 | 节点 | text | data | bss | Flash | 静态 RAM | 构建结果 |
|---|---|---:|---:|---:|---:|---:|---|
| 环境 Smoke Test | F103RB | 4,796 | 12 | 1,572 | 4,808 B / 3.7% | 1,584 B / 7.7% | 0 errors，0 warnings |
| Peripheral baseline | Node A | 13,432 | 12 | 1,908 | 13,444 B / 10.3% | 1,920 B / 9.4% | 0 errors，0 warnings |
| Peripheral baseline | Node B | 9,188 | 12 | 1,732 | 9,200 B / 7.0% | 1,744 B / 8.5% | 0 errors，0 warnings |
| FreeRTOS kernel baseline | Node A | 25,176 | 16 | 13,624 | 25,192 B / 19.2% | 13,640 B / 66.6% | 0 errors，0 warnings |
| FreeRTOS kernel baseline | Node B | 21,840 | 16 | 13,440 | 21,856 B / 16.7% | 13,456 B / 65.7% | 0 errors，0 warnings |
| IPC baseline | Node A | 25,724 | 16 | 13,640 | 25,740 B / 19.6% | 13,656 B / 66.7% | 0 errors，0 warnings |
| IPC baseline | Node B | 22,316 | 16 | 13,448 | 22,332 B / 17.0% | 13,464 B / 65.7% | 0 errors，0 warnings |

较高的静态 RAM 数值主要因为 8192 B FreeRTOS heap 已整体预留在 `.bss`。它不是运行时剩余 heap，也不包含运行过程中最坏栈深和瞬时分配结论。上板后必须记录 `xPortGetFreeHeapSize()`、`xPortGetMinimumEverFreeHeapSize()` 和各任务 stack high-water mark。

对应 Git 基线：

- Node A peripheral：`9d4d0ea`
- Node B peripheral：`88f3d57`
- 双节点 peripheral verification record：`21fc3f6`
- Node A FreeRTOS kernel：`98d271c`
- Node B FreeRTOS kernel：`783a603`
- 双节点 IPC：`4377c5e`

## 13. 当前代码的真实边界

以下项目已经存在于仓库：

- 两个可独立生成和构建的正式 CubeMX/CubeIDE 工程；
- 两节点外设初始化代码；
- FreeRTOS 内核、中间件、5 个 application task 定义；
- 2 个深度 4 的 CAN RX FIFO Queue；
- 3 个长度 1 的 latest-state Mailbox；
- Node A/Node B 各自的 IPC 数据结构；
- Stack Overflow 和 Malloc Failed Hook 入口。

以下项目尚未实现或尚未验证：

- 任务的 10 ms/100 ms 周期控制和 event/deadline blocking；当前循环仍为 `osDelay(1)`；
- CAN filter、`HAL_CAN_Start`、RX notification、ISR-to-Queue callback、Tx submission 和 bus-off/error recovery；
- 0x100/0x180 协议编解码、reserved/range/consistency 检查和 Alive 状态机；
- Queue overwrite/peek/receive 的实际数据流；
- PWM 启动、Tach 捕获处理、INA260 I2C 事务、ADC 校准与 NTC 转换；
- Thermal Strategy、Demand-to-Target-RPM mapping、RPM controller 和 fallback mapping；
- 全部 Fault detection、qualification、dependency/suppression、mode arbitration 和 recovery；
- diagnostic event history、Freeze Frame、NVM、aging、clear 或 PC/UDS exposure；
- Hook 的安全停机、记录和可观察行为；
- 任何硬件运行、时序、波形、传感器精度或故障响应实测。

原计划中的硬件无关 `firmware/common` CAN protocol codec 和主机端单元测试尚未创建。它仍是协议接入任务前的必要工作，不因硬件到货而取消。

## 14. 硬件 Bring-up 安全基线

1. 第一次连接只给单块 NUCLEO 通过 USB 供电，不连接 12 V 风扇、加热器或完整 CAN 网络。
2. 逐件核对模块丝印、引脚方向、风扇线序、DC 插头极性和电阻实测值。图片或商家页面不能代替手中实物标签。
3. 12 V 适配器、Fan、Heater 和两块 NUCLEO 必须建立公共 GND，但 12 V 不得进入 MCU 3.3 V/5 V 信号引脚。
4. Heater 大电流路径使用端子、开关和 18 AWG 功率线，不通过无焊面包板或细杜邦线。
5. CAN 总线断电时，两端各启用一个 120 Ω termination，CANH 与 CANL 间预期约 60 Ω；只有端点终端，不额外叠加。
6. 风扇先单独供 12 V，暂不接 MCU PWM。用万用表测 PWM 线开路电压并对照 PC7 和风扇接口约束；不合适时启用预留晶体管方案。
7. Tach 使用外部 4.7 kΩ 上拉到 3.3 V，再接 PA0。不得把未知风扇内部电压直接假设为 3.3 V。
8. NTC 分压使用 3.3 V；INA260 的逻辑供电和 I2C 电平按模块实物/资料复核后连接。
9. 不通过真实短路、危险堵转、过压或大电流操作制造 Fault。FA-01/04/06 等状态机主线使用安全的软件注入验证。
10. 万用表属于主线必需工具；示波器、逻辑分析仪、USB-CAN、电子负载和可调电源是可选增强。没有测量证据时，只能写“配置为”，不能写“实测为”。

## 15. 下一阶段建议顺序和验收门

### Gate H0 到货验收

- 清点所有实际到货部件、数量和型号；
- 拍摄关键模块正反面与丝印；
- 测量电阻、适配器空载电压和极性；
- 记录缺件、错件、损伤和模块跳线状态。

通过条件：设备身份和基本电气信息明确，尚未进行复杂连接。

### Gate H1 NUCLEO 单板 Bring-up

- 两块板分别通过 USB 上电；
- 确认 Windows 枚举、ST-LINK、虚拟串口和 CubeProgrammer 连接；
- 读取目标器件信息并完成下载、运行、停止、断点和复位；
- 用最小 LED/UART 观察点证明固件确实在目标板运行。

通过条件：两块板分别可稳定下载调试；不把一块板成功外推为另一块板成功。

### Gate H2 低压通信与 CAN 物理层

- 先只连接两块 NUCLEO、两个 CAN Pal、公共地、CANH/CANL 和终端；
- 使用短线和两端终端，断电测量约 60 Ω；
- 从固定帧和计数器开始验证双向收发，再接入正式 0x100/0x180；
- 记录 Tx/Rx counter、ID、DLC、payload、timeout 和 error state。

通过条件：双向连续通信可重复，断开一端能被明确观察；此时仍不宣称 Fault 逻辑完成。

### Gate H3 单外设验证

- Node B：ADC calibration → NTC raw ADC → 电压 → 温度换算和 open/short 边界；
- Node A：PWM 安全检查 → Fan PWM；随后单独接入 Tach 并确定 pulses per revolution；
- Node A：I2C bus scan/设备识别 → INA260 voltage/current 读取；
- 每次只增加一个外设，保留前一阶段可回退提交。

通过条件：每个输入/输出都有独立日志或 Debug evidence，未同时依赖完整闭环才能判断成败。

### Gate H4 FreeRTOS 和 IPC 运行验证

- 把 task placeholder 改为冻结的 periodic/event-driven 结构；
- 检查所有 Queue/Task Handle 非空；
- 记录 task counter、period、jitter、stack high-water mark 和 heap；
- 验证 Queue full、Mailbox overwrite 和超时路径；
- 实现 Stack Overflow/Malloc Failed Hook 的可观察安全行为。

通过条件：调度和 IPC 有运行证据，不仅是编译成功。

### Gate H5 正常闭环

- 完成 common CAN protocol codec 和主机端 golden-vector tests；
- 依次接入 0x100、0x180、Alive、raw/trusted timestamps；
- 完成 Temperature → Cooling Demand → CAN → Fan Target/PWM → RPM → Status 的正常链路；
- 记录温度、Demand、Target RPM、Actual RPM、PWM、Current/Voltage 和 Mode。

通过条件：正常闭环可重复运行，数据与 CAN Matrix 一致，尚未启用的 Fault 不伪装为已验证。

### Gate H6 Fault 和 Recovery

- 先完成软件注入接口，再逐项验证 FA-01～FA-07、FB-01～FB-04、FB-06；
- 覆盖 suspected、confirmed、fallback、recovery、dependency/suppression 和 multi-fault mode recomputation；
- 重点验证 100 ms、300 ms、连续 2 帧和连续 3 帧语义；
- 分离 current runtime state 与 diagnostic event history。

通过条件：每个 Fault 有可复现刺激、预期状态转移、实际日志、恢复证据和结论。

### Gate H7 定量收口和简历证据

- 记录 task period、observed maximum execution time、response time、jitter 和 margin；
- 记录 CAN 周期、timeout、错误恢复和总线基本统计；
- 记录控制响应、稳态误差、温度曲线和 Fault 响应；
- 将“配置值”“计算值”“软件注入结果”“硬件实测值”分别标记。

通过条件：简历中的每个量化结论都能回到代码、日志、测试记录或测量证据。

## 16. 实施和留痕规则

- 每个 Gate 使用小步提交，保持一个问题对应一个可回退 Git 变化；
- CubeMX 手工代码必须放在 `USER CODE` 保护区，独立模块放在明确的 `Inc/Src` 或 common 目录；
- 不提交 `Debug/Release`、ELF、MAP、LIST 和临时测试产物；
- 每次提交前至少完成目标工程 `0 errors, 0 warnings`、`git diff --check` 和 `git status` 检查；
- 保存必要的串口日志、Debugger 变量截图、连接照片、万用表读数和测试表，但不把大量重复图片或构建产物塞入仓库；
- 没有执行的测试写“未验证”，失败的测试保留现象和原因，不用文档措辞掩盖；
- 阈值、Debounce、Observation、Recovery、fallback value、PPR、RPM mapping 等受控 TBD 必须由实测和测试需求驱动，不能为了填表提前编造。

## 17. 下一次工作的明确起点

下一次会话从 Gate H0 开始。第一步不是接线或修改 Fault List，而是把实际收到的部件逐件列出，并提供关键模块正反面、丝印和接口照片。完成实物身份核对后，再给出第一轮只涉及 NUCLEO USB/ST-LINK 的操作清单。

在 H0/H1 完成前，不连接 Heater，不连接完整风扇功率回路，不启用 INA260 ALERT，也不同时调试双节点 CAN 和全部传感器。

## 18. 参考记录

- [开发环境搭建与最小编译验证记录](../05_verification_gates_验证与门禁/Development_Environment_Verification_Record_开发环境验证记录_2026-09-12.md)
- [双节点外设基线配置与编译验证记录](Dual_Node_Peripheral_Baseline_Verification_Record_双节点外设基线验证记录_2026-09-16.md)
- [双 ECU 外设资源与低仪器依赖验证草案](Peripheral_Pin_Clock_Interrupt_Allocation_外设引脚时钟中断分配_v0.1_Working_Draft_工作草案_2026-09-13.md)
- [CAN Matrix Design Decision Record / CAN 矩阵设计决策记录 v0.1](../04_can_communication_CAN通信设计/CAN_Matrix_Design_Decision_Record_CAN矩阵设计决策记录_v0.1_Synced_已同步_2026-09-02.docx)
