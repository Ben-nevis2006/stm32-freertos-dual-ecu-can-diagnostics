# STM32 FreeRTOS 双 ECU CAN 实时通信与故障检测系统

本仓库记录双 NUCLEO-F103RB + TJA1051T/3 CAN Pal 的分阶段硬件验收、低压 CAN bring-up、固件实现和可复核证据。项目采用 Gate 制：每个阶段先定义安全边界和通过条件，再保留原始证据、失败路径与关闭结论。

## 仓库目录地图

| 目录 | 中文含义 | 内容边界 |
|---|---|---|
| [`current/`](current/README.md) | 当前有效文档 | 只放仍有效的设计、基线、协议、Gate 记录和工程复盘，并按六个板块分类 |
| [`archive/`](archive/README.md) | 历史版本归档 | 保存已被替代的旧版本，仅用于追溯，不作为当前执行依据 |
| [`evidence/`](evidence/) | 原始证据 | 保存照片、枚举记录、UART 日志、哈希与按日期组织的现场证据 |
| [`firmware/`](firmware/) | 固件源码 | 保存 Node A、Node B 的可构建 STM32CubeIDE 工程 |

`current/` 内采用 `序号_English_中文` 分类目录和 `English_中文_[版本]_[状态]_[日期]` 文件名；根级技术目录保留稳定英文路径，中文释义由本表和各级索引提供，避免破坏工程脚本与历史引用。

## 当前状态

| Gate | 状态 | 结果 |
|---|---|---|
| H0 硬件到货与身份 | CONDITIONAL PASS | CAN/H2 身份子项已关闭；其他外设随对应 Gate 继续验证 |
| H1 NUCLEO USB Bring-up | PASS | 两板独立枚举、目标识别、Flash 访问与稳定性通过 |
| H2 低压 CAN 物理链路 | **PASS（2026-10-07）** | 500 kbit/s；固定帧 3×80 s；正式 `0x100/0x180`；受控断线与恢复通过 |
| H3 单外设验证 | **IN PROGRESS** | Node B NTC 子项 H3-00～H3-05 已于 2026-10-10 关闭；`FAN-00A`、`PSU-00B / D-04`、`FAN-00C` 已通过，当前仅执行 `FAN-00D` 接电前针脚/接法确认，尚未放行 12 V 接入风扇 |

H2 仅覆盖 USB/3.3 V 低压双节点链路。`12 V`、Fan、Heater、INA260、NTC、完整 timeout/fault 状态机、负载耐久与 EMC 尚未由 H2 证明。

## H2 推进时间线

本次 GitHub 更新按用户约定在 Gate H2 完成后**统一推送一次**，但工作不是一次性完成：

- `2026-10-02`：收发器身份、终端开关与无源总线起点；
- `2026-10-03`：排针焊接、Node A 供电与接线核对；
- `2026-10-04`：Node B 供电、USB 稳定性、设备身份固化；
- `2026-10-05`：最小 CAN 固件构建/刷写；首次运行失败并进入 HOLD；
- `2026-10-07`：定位焊锡桥连、跳线/SLNT 接触和时钟基准问题；完成固定帧、正式帧、断线注入与恢复验收。

历史失败记录没有被最终 PASS 覆盖，也没有伪造或回填历史 Git 提交时间。

## H3 NTC 推进时间线

H3 仍采用“阶段内本地累计、整个 Gate 完成后统一推送”策略；以下日期是实际动作日期，不等于未来统一推送时间：

- `2026-10-07`：冻结 NTC 拓扑、换算模型和安全边界，完成第一轮板外无源检查；
- `2026-10-08`：关闭机械/线色风险，完成接板预检、ADC 实现、定向刷写、室温基线与 Run01；
- `2026-10-09`：完成自行计时 Run02、恢复延长段、旧固件开/短路样本表征与在路阻值排查；
- `2026-10-10`：实现 OPEN/NORMAL/SHORT 三态，完成构建、定向刷写、开路/旁路/最终恢复验收，关闭 NTC 子项。

NTC 关闭不等于整个 H3 关闭；Fan PWM、Tach/PPR 和 INA260 仍需分别准入与验证。

## 关键入口

- [当前阶段文档索引](current/README.md)
- [Gate H2 执行记录](current/05_verification_gates_验证与门禁/Gate_H2_CAN_Physical_Layer_Execution_Record_H2_CAN物理层执行记录_2026-10-02.md)
- [H2 测试协议与验收结果](current/05_verification_gates_验证与门禁/H2_CAN_Minimal_Test_Protocol_H2_CAN最小测试协议_2026-10-05.md)
- [H2 排障证据复盘](current/06_engineering_recaps_工程复盘/H2_CAN_Bringup_Troubleshooting_Evidence_Recap_H2_CAN联调排障证据复盘_2026-10-07.md)
- [H2 2026-10-07 原始运行日志与 SHA-256](evidence/software/H2/2026-10-07/README.md)
- [Gate H3 单外设验证执行记录](current/05_verification_gates_验证与门禁/Gate_H3_Single_Peripheral_Validation_Execution_Record_H3单外设验证执行记录_2026-10-07.md)
- [H3 2026-10-07 准入证据索引](evidence/hardware/H3/2026-10-07/README.md)
- [H3 2026-10-08 H3-01 关闭证据索引](evidence/hardware/H3/2026-10-08/README.md)
- [H3 2026-10-08 ADC 构建、刷写与运行证据索引](evidence/software/H3/2026-10-08/README.md)
- [H3 2026-10-09 Run02 与边界表征证据](evidence/software/H3/2026-10-09/README.md)
- [H3 2026-10-10 边界实现与 NTC 子项关闭证据](evidence/software/H3/2026-10-10/README.md)
- [H3 NTC 联调排障证据复盘](current/06_engineering_recaps_工程复盘/H3_NTC_Bringup_Troubleshooting_Evidence_Recap_H3_NTC联调排障证据复盘_2026-10-10.md)
- [Node A 固件工程](firmware/node_a)
- [Node B 固件工程](firmware/node_b)

## H2 实测摘要

- 双端 termination 并联：最终从两端分别测得 `60.3 Ω / 60.4 Ω`。
- 正式基线：Node B→A `0x100/DLC2`，Node A→B `0x180/DLC5`，周期 `100 ms`，4 位 AliveCounter。
- 正式 80 s：Node A `tx/rx=687/688`，Node B `690/689`，双方 `tx_fail/rx_bad/seq_err/q_drop=0`。
- 断开 H/L：接收归零、发送失败及 Error Warning/Passive/Bus-off 可观察。
- 最终恢复 80 s：双方四项错误为 0，max TEC/REC 均为 `6/0`，无 Warning/Passive/Bus-off，末态 ESR=0。

所有数值均可回到仓库原始 UART 日志与哈希索引复核。
