# 当前有效文档 / Current Effective Documents

`current/` 只保存当前仍有效、可直接用于设计或验收的文档。被替代的旧版本进入 [`archive/`](../archive/README.md)，原始照片、日志和哈希进入 [`evidence/`](../evidence/)，可构建源码进入 [`firmware/`](../firmware/)；同一文档不在多个板块重复复制。

## 命名与归档规则

- 分类目录：`序号_English_中文`，兼顾工具排序和中文阅读。
- 有效文件：`English_中文_[版本]_[状态]_[日期].扩展名`。
- `Synced_已同步` 表示已与当时设计基线同步；`Working_Baseline_工作基线` 表示当前可执行但仍允许迭代；`Working_Draft_工作草案` 表示尚未冻结。
- 文件日期表示内容形成或执行日期，不等同于 Git 提交或统一推送日期。
- 历史文件不因整理而改写时间；失效版本只移动到 `archive/`，不删除。

## 01 Governance / 项目治理

- [Architecture Synchronization Record / 架构同步记录（2026-09-03）](01_governance_项目治理/Architecture_Synchronization_Record_架构同步记录_2026-09-03.docx)

## 02 Architecture / 系统架构

- [System Architecture Definition / 系统架构定义 v0.1（已同步）](02_architecture_系统架构/System_Architecture_Definition_系统架构定义_v0.1_Synced_已同步_2026-09-02.docx)
- [FreeRTOS Task Architecture / FreeRTOS 任务架构 v0.1（已同步）](02_architecture_系统架构/FreeRTOS_Task_Architecture_FreeRTOS任务架构_v0.1_Synced_已同步_2026-09-02.docx)
- [Fault List / 故障清单 v0.1（已同步）](02_architecture_系统架构/Fault_List_故障清单_v0.1_Synced_已同步_2026-09-02.docx)

## 03 Hardware & Interfaces / 硬件与接口

- [Hardware Selection / 硬件选型 v0.1（工作基线）](03_hardware_interfaces_硬件与接口/Hardware_Selection_硬件选型_v0.1_Working_Baseline_工作基线_2026-09-08.docx)
- [Peripheral, Pin, Clock & Interrupt Allocation / 外设、引脚、时钟与中断分配 v0.1（工作草案）](03_hardware_interfaces_硬件与接口/Peripheral_Pin_Clock_Interrupt_Allocation_外设引脚时钟中断分配_v0.1_Working_Draft_工作草案_2026-09-13.md)
- [Dual-node Peripheral Baseline Verification / 双节点外设基线验证记录（2026-09-16）](03_hardware_interfaces_硬件与接口/Dual_Node_Peripheral_Baseline_Verification_Record_双节点外设基线验证记录_2026-09-16.md)
- [Hardware Arrival Transition Report / 硬件到货交接报告（2026-09-17）](03_hardware_interfaces_硬件与接口/Hardware_Arrival_Transition_Report_硬件到货交接报告_2026-09-17.md)

## 04 CAN Communication / CAN 通信设计

- [CAN Matrix / CAN 矩阵 v0.1（Excel，已同步）](04_can_communication_CAN通信设计/CAN_Matrix_CAN矩阵_v0.1_Synced_已同步_2026-09-02.xlsx)
- [CAN Matrix / CAN 矩阵 v0.1（Word，已同步）](04_can_communication_CAN通信设计/CAN_Matrix_CAN矩阵_v0.1_Synced_已同步_2026-09-02.docx)
- [CAN Matrix Design Decision Record / CAN 矩阵设计决策记录 v0.1（已同步）](04_can_communication_CAN通信设计/CAN_Matrix_Design_Decision_Record_CAN矩阵设计决策记录_v0.1_Synced_已同步_2026-09-02.docx)

## 05 Verification & Gates / 验证与门禁

- [Development Environment Verification / 开发环境验证记录（2026-09-12）](05_verification_gates_验证与门禁/Development_Environment_Verification_Record_开发环境验证记录_2026-09-12.md)
- [Gate H0 Hardware Acceptance Baseline / H0 硬件验收基线（2026-09-22）](05_verification_gates_验证与门禁/Gate_H0_Hardware_Acceptance_Baseline_H0硬件验收基线_2026-09-22.md)
- [Gate H1 NUCLEO USB Bring-up Execution Record / H1 NUCLEO USB 联调执行记录（2026-09-24）](05_verification_gates_验证与门禁/Gate_H1_NUCLEO_USB_Bringup_Execution_Record_H1_NUCLEO_USB联调执行记录_2026-09-24.md)
- [Gate H2 CAN Physical-layer Execution Record / H2 CAN 物理层执行记录（2026-10-02～10-07）](05_verification_gates_验证与门禁/Gate_H2_CAN_Physical_Layer_Execution_Record_H2_CAN物理层执行记录_2026-10-02.md)
- [H2 CAN Minimal Test Protocol / H2 CAN 最小测试协议与验收结果（2026-10-05～10-07）](05_verification_gates_验证与门禁/H2_CAN_Minimal_Test_Protocol_H2_CAN最小测试协议_2026-10-05.md)
- [Gate H3 Single-peripheral Validation Execution Record / H3 单外设验证执行记录（2026-10-07～）](05_verification_gates_验证与门禁/Gate_H3_Single_Peripheral_Validation_Execution_Record_H3单外设验证执行记录_2026-10-07.md)

## 06 Engineering Recaps / 工程复盘

- [H2 CAN Bring-up Troubleshooting Evidence Recap / H2 CAN 联调排障证据复盘（2026-10-02～10-07）](06_engineering_recaps_工程复盘/H2_CAN_Bringup_Troubleshooting_Evidence_Recap_H2_CAN联调排障证据复盘_2026-10-07.md)
- [H3 NTC Bring-up Troubleshooting Evidence Recap / H3 NTC 联调排障证据复盘（2026-10-07～10-10）](06_engineering_recaps_工程复盘/H3_NTC_Bringup_Troubleshooting_Evidence_Recap_H3_NTC联调排障证据复盘_2026-10-10.md)

## 当前 Gate 状态

| Gate | 状态 | 有效结论与证据 |
|---|---|---|
| H0 硬件到货与身份 | CONDITIONAL PASS | CAN/H2 身份子项已关闭；[原始证据](../evidence/hardware/H0/) |
| H1 NUCLEO USB Bring-up | PASS | 两板独立枚举、目标识别、Flash 访问与稳定性通过；[原始证据](../evidence/hardware/H1/) |
| H2 低压 CAN 物理链路 | **PASS（2026-10-07）** | 500 kbit/s、正式帧、受控断线与恢复通过；[2026-10-05 首次失败](../evidence/software/H2/2026-10-05/README.md)、[2026-10-07 修复与验收](../evidence/software/H2/2026-10-07/README.md) |
| H3 单外设验证 | **IN PROGRESS / HARDWARE HOLD** | Node B NTC `H3-00～H3-05`、Node A Fan PWM `FAN-00A～FAN-02B` 与 Tach `TACH-00B～TACH-02B` 均已关闭为 PASS；TACH-01A/01B 的失败路径仍完整保留。INA260-01 已完成四线无源准入、3.3 V 静态上电、主控状态、全地址扫描、面包板端/模块排针端波形与 A0/A1 地址网络复核；约 `100 kHz` 地址事务到达模块排针但全地址无 ACK。2026-10-21 又由电脑只复位并读取 COM7，连续两次 `0x40` 探测及完整扫描仍为零 ACK，结论保持模块级无应答，等待替换/独立修复后从 `INA-03A` 重新准入。CAN RPM 正常链路属于 H5，FA-01/02/03 属于 H6，不作为 H3 阻塞项。[执行记录](05_verification_gates_验证与门禁/Gate_H3_Single_Peripheral_Validation_Execution_Record_H3单外设验证执行记录_2026-10-07.md)、[10-21 INA260 电脑复扫](../evidence/software/H3/2026-10-21/README.md)、[10-20 INA260 硬件证据](../evidence/hardware/H3/2026-10-20/README.md)、[10-20 Tach/INA260 软件证据](../evidence/software/H3/2026-10-20/README.md)、[10-19 Tach 软件证据](../evidence/software/H3/2026-10-19/README.md)、[10-14 Tach 硬件证据](../evidence/hardware/H3/2026-10-14/README.md)、[10-14 Tach 构建证据](../evidence/software/H3/2026-10-14/README.md)、[10-13 FAN 硬件证据](../evidence/hardware/H3/2026-10-13/README.md)、[10-13 FAN 软件证据](../evidence/software/H3/2026-10-13/README.md) |

> Gate H2 采用“阶段内本地累计、Gate 完成后统一推送”策略。远端关闭提交汇总 2026-10-02～10-07 的连续推进，各记录保留真实发生日期；失败、修复和验收没有被压缩成同一天的一次操作。

> Gate H3 延续同一策略。NTC 子项实际跨越 2026-10-07～10-10，Fan PWM 子项跨越 2026-10-10～10-13，Tach/PPR 自 2026-10-13 起继续推进；TACH-01A 于 2026-10-14 构建、2026-10-19 实板否决，TACH-01B 于 10 月 19 日收口为“明显改善但周期完整性仍失败”，TACH-01C、TACH-02A 与 TACH-02B 于 10 月 20 日依次关闭原始捕获、PPR/量程和 RPM/新鲜度；INA260-01 于 10 月 20 日由身份/无源准入推进到模块级 NACK 隔离，并于 10 月 21 日由电脑端复位复扫独立复现。各动作的真实日期和失败路径原样保留。由于 INA260 已进入等待替换件的硬件暂挂状态，用户于 2026-10-21 授权将现有 H3 证据做一次阶段性提交并同步远端；该同步不代表 H3 关闭，替换模块到位后仍从 `INA-03A` 继续。CAN 正常链路与 Fault/Recovery 分别按 H5/H6 推进。
