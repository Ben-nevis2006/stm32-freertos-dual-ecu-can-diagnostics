# 双 ECU 外设资源与低仪器依赖验证草案

## 1. 文档信息

| 项目 | 内容 |
|---|---|
| 项目名称 | 基于 STM32 + FreeRTOS 的双 ECU CAN 实时通信与故障检测系统 |
| 文档版本 | v0.1 Working Draft |
| 编制日期 | 2026-09-13 |
| 适用硬件 | NUCLEO-F103RB × 2（STM32F103RBT6） |
| 当前阶段 | 硬件到货前的软件环境与正式工程准备 |
| 状态 | 资源与验证方法草案，不修改已冻结项目范围 |

## 2. 约束与修订原则

项目功能范围已经由 SAD、CAN Matrix、FreeRTOS Task Architecture 和 Fault List 冻结。本文件只能为这些冻结需求分配 MCU 资源和设计可执行的验证方法，不能擅自删除、合并或降级 Fault。

本次按大学生实习项目的实际条件做的是“降低验证设备依赖”，不是“减少项目内容”：

- 完整保留 `FA-01` 至 `FA-07`、`FB-01` 至 `FB-04`、`FB-06`；
- 保留 `FB-05` 为历史编号和 Fault Propagation 机制，不把它实现成独立 Fault；
- 保留 NORMAL、DEGRADED、SAFE、Fallback、Qualification、Recovery 和多故障依赖关系；
- 阈值、Debounce、部分恢复时间等继续保持 Fault List 中的受控 TBD；
- 不强制依赖示波器、CAN 分析仪、电子负载或可调实验电源；
- 主要使用软件故障注入、串口日志、CubeIDE Debug/Watch、板载 LED 和安全的低压拔插完成验证；
- 没有实际测量的数据，不写成“已经实测”。

## 3. 设计输入优先级

1. `System_Architecture_Definition_v0.1_Synced_2026-09-02.docx`；
2. `Fault_List_v0.1_Synced_2026-09-02.docx`；
3. `CAN_Matrix_v0.1_Synced_2026-09-02.docx/.xlsx`；
4. `FreeRTOS_Task_Architecture_v0.1_Synced_2026-09-02.docx`；
5. `Hardware_Selection_v0.1_Working_Baseline_2026-09-08.docx`；
6. `Architecture_Synchronization_Record_2026-09-03.docx`。

旧版 `project charter.docx` 只用于说明立项目的。它包含的独立 Heartbeat、多条 10 ms 周期报文以及每个功能单独建 Task 等早期设想，已被上述同步文档替代。

## 4. 两节点公共配置

| 功能 | 外设或引脚 | 板端位置 | 初始设置 |
|---|---|---|---|
| SWD 调试 | PA13/SWDIO、PA14/SWCLK | 板载 ST-LINK | `SYS > Debug = Serial Wire` |
| CAN 接收 | CAN1_RX / PA11 | CN10-14 | CAN1 默认映射 |
| CAN 发送 | CAN1_TX / PA12 | CN10-12 | CAN1 默认映射 |
| 调试串口 | USART2_TX / PA2、USART2_RX / PA3 | 板载 ST-LINK 虚拟串口 | 115200-8-N-1 |
| 状态灯 | PA5 / LD2 | 板载绿灯 | 粗粒度状态提示 |
| 人工测试入口 | PC13 / B1 | 板载用户按键 | 可选测试模式，不参与正常控制 |
| FreeRTOS Tick | SysTick | MCU 内部 | FreeRTOS 使用 |
| HAL Tick | TIM4 | MCU 内部 | 启用 FreeRTOS 时使用 TIM4 |

USART2 用于低频结构化日志，不能在 10 ms 控制任务内连续阻塞打印。B1 只用于开发版本触发安全的软件注入，不改变正式运行逻辑。

## 5. Node A 外设资源

| 功能 | MCU 资源 | 板端位置 | 初始配置 |
|---|---|---|---|
| CAN RX | CAN1_RX / PA11 | CN10-14 | 默认映射 |
| CAN TX | CAN1_TX / PA12 | CN10-12 | 默认映射 |
| 风扇 PWM | TIM3_CH2 / PC7 | D9 / CN10-19 | TIM3 Full Remap；AF Open-Drain；25 kHz |
| 风扇 Tach | TIM2_CH1 / PA0 | A0 / CN7-28 | Input Capture；下降沿；外接 3.3 V、4.7 kΩ 上拉 |
| INA260 SCL | I2C1_SCL / PB8 | D15 / CN10-3 | I2C1 Remap；100 kHz 起步 |
| INA260 SDA | I2C1_SDA / PB9 | D14 / CN10-5 | I2C1 Remap；100 kHz 起步 |
| INA260 ALERT | GPIO_EXTI5 / PB5 | D4 / CN10-29 | 预留快速本地保护入口，硬件基础读取通过后再启用 |

### 5.1 风扇 PWM

不使用 `PA6 / D12` 直接连接风扇 PWM，因为 PA6 不是 5 V tolerant。`PC7 / D9` 是 5 V tolerant，可通过 TIM3 Full Remap 输出 PWM。

硬件到货后，先给风扇供电但暂不接 MCU，用普通万用表测量 PWM 线开路电压。若电压符合 PC7 输入约束，则采用开漏直连；若不符合，再启用硬件选型中保留的晶体管方案。这是唯一不可省略的仪器性电气安全检查。

### 5.2 Tach 与 INA260

PWM 与 Tach 分别使用 TIM3、TIM2，使 25 kHz PWM 和低频转速周期捕获拥有独立计数尺度。PPR、Stall 阈值和速度偏差阈值继续保持 TBD，硬件到货后通过实际运行数据标定。

INA260 负责 Motor Current 和 Supply Voltage，因此同时支持 FA-04、FA-05、FA-06 以及其他故障的上下文判断。第一阶段先完成普通 I2C 读取；随后再接入 ALERT 或在 Node A 本地周期路径中执行严重过流判断。无论采用哪条实现路径，本地保护不得等待 CAN。

## 6. Node B 外设资源

| 功能 | MCU 资源 | 板端位置 | 初始配置 |
|---|---|---|---|
| CAN RX | CAN1_RX / PA11 | CN10-14 | 默认映射 |
| CAN TX | CAN1_TX / PA12 | CN10-12 | 默认映射 |
| NTC 电压 | ADC1_IN0 / PA0 | A0 / CN7-28 | 单通道、软件触发、55.5 cycles |

NTC 每 100 ms 采集一次即可，不使用 DMA。上电后执行 ADC 校准。过温、恢复回差、开路/短路和不足冷却判据仍按 Fault List 保持 TBD，后续根据采样数据选择能够稳定演示的数值，但不改变 Fault 定义。

## 7. 时钟和 CAN 位时序

当前沿用已经完成编译验证的 Nucleo 默认 64 MHz HSI 方案，以减少硬件到货前的无效配置工作：

| 项目 | 设置 |
|---|---:|
| SYSCLK / HCLK | 64 MHz / 64 MHz |
| PCLK1 | 32 MHz |
| APB1 Timer Clock | 64 MHz |
| PCLK2 | 64 MHz |
| ADC Clock | PCLK2 / 6，约 10.67 MHz |

CAN1 工作基线为 500 kbit/s：

| 参数 | 值 |
|---|---:|
| Prescaler | 4 |
| Time Segment 1 | 13 TQ |
| Time Segment 2 | 2 TQ |
| SJW | 1 TQ |
| 实际波特率 | 500 kbit/s |
| 采样点 | 87.5% |

Node A 定时器初值：

- TIM3 PWM：PSC = 0，ARR = 2559，配置频率为 25 kHz；
- TIM2 Tach：PSC = 639，计数频率为 100 kHz，ARR = 65535。

两块板必须使用相同的 CAN 参数。若实物在短线、室温台架上通信稳定，主线不再增加 HSE 改造；若通信实测出现时钟相关问题，再依据证据调整。未用示波器或逻辑分析仪测量时，报告中应写“定时器配置为 25 kHz”，不能写“实测波形为 25 kHz”。

## 8. 中断和任务约束

| 中断 | 暂定优先级 | ISR 责任 |
|---|---:|---|
| INA260 ALERT / EXTI9_5 | 4 | 可选快速保护；只锁存、记录和执行已定义的最小本地动作，不调用 FreeRTOS API |
| CAN1_RX0 | 5 | 将帧放入深度 4 的队列，不解析业务 |
| TIM2 Input Capture | 6 | 保存捕获值或周期，不在 ISR 中做故障判定 |
| TIM4 HAL Timebase | 15 | HAL 1 ms Tick |
| SysTick | 15 | FreeRTOS Tick |

任务架构保持冻结：

- Node A：`A_ControlTask` 10 ms、`A_ComRxTask` 事件触发、`A_Cyclic100msTask` 100 ms；
- Node B：`B_ComRxTask` 事件触发、`B_Cyclic100msSupervisorTask` 100 ms；
- 不新增 SensorTask、DiagnosisTask 或 HeartbeatTask；
- Fault Detection 是上述任务内调用的模块，不等于为每个 Fault 建一个任务；
- runtime latest state 与 diagnostic event history 分离；第一版可使用固定长度 RAM event log，不要求 NVM 或 UDS。

## 9. 冻结 Fault List 完整映射

### 9.1 Node A 本地故障

| ID | Fault | 模式影响 | 主要实现输入或资源 |
|---|---|---|---|
| FA-01 | Fan Stall | SAFE | Fan request、Target RPM、TIM2 Actual RPM、INA260 Current/Voltage context |
| FA-02 | Fan Speed Deviation | Level 1 DEGRADED；Level 2 SAFE | Target RPM、Actual RPM、Supply Voltage、Control Mode |
| FA-03 | RPM Feedback Fault | Fallback 可用时 DEGRADED，否则 SAFE | Tach pulse、Fan command、Current/Voltage plausibility context |
| FA-04 | Motor Overcurrent | Severe 时 SAFE | INA260 Current、Drive State、Current validity；Node A 本地保护不等待 CAN |
| FA-05 | Current Feedback Fault | DEGRADED | INA260 通信和 Current plausibility；失效后禁用依赖电流的诊断 |
| FA-06 | Supply Voltage Fault | DEGRADED 或 SAFE | INA260 Bus Voltage、Fan performance/current context |
| FA-07 | Cooling Demand Communication Fault | DEGRADED 或 SAFE | `0x100` timing、4-bit Alive/sequence、validity、last valid demand |

### 9.2 Node B 系统故障

| ID | Fault | 模式影响 | 主要实现输入或资源 |
|---|---|---|---|
| FB-01 | Overtemperature | SAFE | Valid Plant Temperature |
| FB-02 | Insufficient Cooling Performance | DEGRADED，可发展到 SAFE | Plant Temperature、Cooling Demand、Node A/actuator availability、可选 dT/dt |
| FB-03 | Node A Communication Timeout | SAFE | `0x180` raw reception timing/freshness |
| FB-04 | Node A Alive / Sequence Fault | DEGRADED，可升级 SAFE | `0x180` 4-bit Alive/sequence、trusted freshness |
| FB-06 | Plant Temperature Signal Fault | SAFE | NTC raw/conditioned value、range、plausibility、freshness |

`FB-05` 不作为独立 Fault。Node A 上报的是 underlying FA Fault、signal validity、control/fallback mode 和 availability，Node B 据此聚合系统模式。

### 9.3 已冻结的通信诊断参数

| 项目 | 冻结值 |
|---|---|
| `0x100 Cooling_Command` 周期 | 100 ms |
| `0x180 Actuator_Status` 周期 | 100 ms |
| raw/valid/trusted freshness timeout | 300 ms |
| Alive | 4 bit |
| Sequence anomaly | 单次 suspected/re-sync；连续 2 次 Confirm |
| Communication recovery | 连续 3 帧 valid/trusted 后恢复 |
| FA-07 Qualification 期间 | Hold Last Valid Cooling Demand |
| FA-07 Confirmed 后 | Local Conservative Cooling Demand；RPM 有效时保留本地速度闭环 |

### 9.4 依赖、抑制与恢复规则

- FA-06 Undervoltage 若足以解释速度下降，可抑制 FA-02 独立 Qualification，但保留 RPM evidence；
- FA-03 ACTIVE 时，FA-01 Stall 判断降级/受限，FA-02 正常诊断不可用；
- FA-05 ACTIVE 时，FA-04 软件过流诊断不可用，基于电流的 FA-01 discrimination 降级，但可信 RPM 闭环仍可工作；
- FB-06 ACTIVE 时，FB-01、FB-02 因温度输入无效而不可用，系统因 Observability Loss 进入 SAFE；
- FA-02、FB-02、FB-01 属于执行器、系统冷却功能、热边界三个抽象层，可以同时成立；
- Mode 由全部 Active Confirmed Fault、Functional Availability、Observability 和 Protection State 共同重算，不按故障数量决定；
- Fault 样本消失不等于恢复，必须经过 Recovery Qualification；运行模式恢复也不等于清除历史事件。

## 10. 低仪器依赖验证方法

### 10.1 主要观察通道

1. USART2 输出结构化日志：时间戳、Node、Fault ID、Suspected/Confirmed/Recovered、System Mode、Fallback Mode、关键输入；
2. CubeIDE Live Expressions/Watch 查看 Fault Set、Signal Validity、Alive、计时器和任务计数；
3. 板载 LED 只显示节点存活或 SAFE 状态，不承担详细诊断；
4. 两个节点互为 CAN 测试端，不把 USB-CAN 分析仪作为主线依赖；
5. 对 100 ms、300 ms、连续 2/3 帧等时间语义，使用 FreeRTOS tick 和内部时间戳形成日志证据。

### 10.2 Fault 注入与验证

| Fault | 不依赖专用仪器的验证方法 | 主要验收现象 |
|---|---|---|
| FA-01 Fan Stall | 测试构建注入 `run=true、RPM≈0、Current loaded、Voltage valid` 的输入快照；不要求真实堵转风扇 | Node A 本地 Safe Output；报告 FA-01；Node B 聚合 SAFE |
| FA-02 Speed Deviation | 注入 Level 1/Level 2 的 Target/Actual RPM 偏差并维持相应观察时间 | 分别进入 DEGRADED/SAFE；恢复后重新 Qualification |
| FA-03 RPM Feedback Fault | 注入 Tach missing、frozen、out-of-range 或 implausible jump | `RPM_VALID=false`；进入 Open-loop Fallback；不得伪造 RPM |
| FA-04 Motor Overcurrent | 注入高于 warning/severe 阈值的 Current sample；禁止通过短路制造真实过流 | Severe 时 Node A 不等待 CAN 即进入本地 Safe Output |
| FA-05 Current Feedback Fault | 断开 INA260 低压 I2C 线或注入 I2C error/frozen current | Current invalid；电流相关诊断禁用；可信 RPM 闭环保留 |
| FA-06 Supply Voltage Fault | 注入 UV/OV 样本，不要求可调电源 | DEGRADED/SAFE 符合 severity；可验证对 FA-02 的 root-cause suppression |
| FA-07 Command Communication Fault | 暂停 Node B 的 `0x100` 发送，或测试构建故意重复/跳变 Alive | Hold Last Valid；连续 2 次异常或 300 ms 后 Confirm；连续 3 帧后恢复 |
| FB-01 Overtemperature | 软件注入高温值；硬件阶段可用已知电阻或安全加热 NTC 辅助验证 | SAFE；发送 Emergency/Maximum-like Cooling；回差后恢复 Qualification |
| FB-02 Insufficient Cooling | 注入“高 Demand、A healthy、温度长期未改善”的温度序列 | DEGRADED；若继续发展到 FB-01 则 SAFE |
| FB-03 Node A Timeout | 暂停 Node A 的 `0x180` 发送或断开 CAN 低压逻辑链路 | 300 ms Confirm SAFE；连续 3 帧 trusted Status 后恢复 |
| FB-04 Alive/Sequence Fault | 测试构建使 `0x180` Alive 重复或跳变，但继续发帧 | 连续 2 次异常 Confirm DEGRADED；300 ms 无 trusted Status 升级 SAFE |
| FB-06 Temperature Signal Fault | 断开/短接 3.3 V NTC 分压支路，或注入 out-of-range/frozen 样本 | Temperature invalid；FB-01/02 unavailable；SAFE 与保守冷却 |

软件故障注入只存在于测试构建，例如使用 `FAULT_INJECTION_ENABLED` 编译开关和固定接口；生产演示构建默认关闭。它用于验证 Fault Manager 行为，不伪装成真实传感器故障实测。

### 10.3 不强制购买或使用的设备

- 示波器或逻辑分析仪：不是主线验收条件；没有它们就不声明 PWM 波形、边沿抖动或微秒级响应已经实测；
- USB-CAN 分析仪：两块 ECU 可互测周期、Alive 和超时；分析仪只作为后续加分项；
- 电子负载或可调电源：过流、欠压和过压的状态机使用安全的软件注入，不通过危险操作制造；
- 转速计和精密温度仪：初版使用传感器内部一致性与合理范围验证；绝对误差标定留作可选工作。

主线仍需要一只普通万用表检查供电、公共地和风扇 PWM 开路电压。这属于基本接线安全，而不是复杂实验室验证。

## 11. 实际开发顺序

1. 将正式 Git 仓库一次性放到纯英文、无空格路径；
2. 创建 Node A `.ioc`，配置 CAN、PWM、Tach、I2C、调试串口并完成空工程构建；
3. 创建 Node B `.ioc`，配置 CAN、ADC、调试串口并完成空工程构建；
4. 启用 FreeRTOS，建立冻结的任务和深度 4 CAN RX 队列；
5. 先实现驱动层和正常通信，再实现完整 Fault List；
6. 给 Fault Manager 增加仅测试构建可用的注入接口；
7. 依次验证单 Fault、Recovery、Dependency/Suppression 和多 Fault Mode 重算；
8. 硬件到货后按供电/下载、单外设、CAN、RTOS、故障闭环的顺序集成；
9. 保存构建日志、串口记录、关键状态截图和测试表，形成简历可举证材料。

推荐仓库结构：

```text
D:\STM32\stm32-dual-ecu-can\
├── current\
├── archive\
└── firmware\
    ├── node_a\
    └── node_b\
```

## 12. 继续保持 TBD 的内容

以下内容不是删除，而是遵照 Fault List 暂不凭空决定：

- Stall、Speed Deviation、Overcurrent、UV/OV、Overtemperature 等阈值；
- Startup grace、Debounce、Observation、Recovery Qualification 时间；
- Open-loop PWM map、Local Conservative Demand、Emergency Cooling Demand；
- 每类 Fault 的 retry、latching、power-cycle 和 diagnostic-clear 策略；
- DTC code、Freeze Frame 字段、NVM、aging、clear 和 UDS/PC exposure；
- 精确任务抖动、PWM 波形和硬件级故障响应时间。

这些项目必须由后续实现数据、硬件现象或测试需要驱动，而不是为了文档完整度提前编造。

## 13. 参考资料

- 当前 `Fault_List_v0.1_Synced_2026-09-02.docx`
- 当前 `System_Architecture_Definition_v0.1_Synced_2026-09-02.docx`
- 当前 `CAN_Matrix_v0.1_Synced_2026-09-02.docx/.xlsx`
- 当前 `FreeRTOS_Task_Architecture_v0.1_Synced_2026-09-02.docx`
- STMicroelectronics, [STM32 Nucleo-64 boards User Manual UM1724](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf)
- STMicroelectronics, [STM32F103x8/xB Datasheet](https://www.st.com/resource/en/datasheet/stm32f103rb.pdf)
- STMicroelectronics, [STM32F1 Reference Manual RM0008](https://www.st.com/resource/en/reference_manual/CD00171190-.pdf)
- Texas Instruments, [INA260 Datasheet](https://www.ti.com/lit/ds/symlink/ina260.pdf)

## 14. 当前结论

冻结的项目架构和 Fault List 全部保留。本草案只把验证方式从“依赖完整实验室仪器”改成“以安全的软件注入、双节点互测、串口日志和调试器观察为主”。

项目最终仍应实现完整故障行为、Fallback、Mode、Recovery 和依赖抑制；但对没有仪器实际测过的波形、精度和微秒级响应保持诚实，不把配置值写成实测结论。
