# 当前版本

本目录保存当前有效的项目文档。

## 阶段记录

- [2026-10-02～10-07：Gate H2 低压通信与 CAN 物理层执行记录（PASS；固定帧 3×80 s、正式 `0x100/0x180`、受控断线与恢复均通过）](Gate_H2_CAN_Physical_Layer_Execution_Record_2026-10-02.md)；[测试协议与验收结果](H2_CAN_Minimal_Test_Protocol_2026-10-05.md)；[排障证据复盘](H2_CAN_Bringup_Troubleshooting_Evidence_Recap_2026-10-07.md)；[2026-10-05 构建/首次失败](../evidence/software/H2/2026-10-05/README.md)；[2026-10-07 修复/验收日志](../evidence/software/H2/2026-10-07/README.md)
- [2026-09-24～10-01：Gate H1 单块 NUCLEO USB Bring-up 执行记录（PASS；两块板均完成独立枚举、稳定性、目标识别、Flash 只读访问与正常断开）](Gate_H1_NUCLEO_USB_Bringup_Execution_Record_2026-09-24.md)；[2026-09-27 NUCLEO-01 上电前、线缆与 Windows 枚举证据](../evidence/hardware/H1/2026-09-27/README.md)；[2026-09-29 NUCLEO-01 闭环与 NUCLEO-02 预检/Windows 枚举证据](../evidence/hardware/H1/2026-09-29/README.md)；[2026-10-01 NUCLEO-02 CubeProgrammer 闭环证据](../evidence/hardware/H1/2026-10-01/README.md)
- [2026-09-23～10-02：Gate H0 硬件到货验收及 D-02 补证记录（CONDITIONAL PASS；CAN/H2 身份子项已关闭，其余外设子项按对应 Gate 继续开放）](Gate_H0_Hardware_Acceptance_Baseline_2026-09-22.md)；[初始原图与 SHA-256](../evidence/hardware/H0/2026-09-23/README.md)；[身份、铭牌与关闭补充原图](../evidence/hardware/H0/2026-09-24/README.md)；[2026-10-01 背面补充原图](../evidence/hardware/H0/2026-10-01/README.md)；[2026-10-02 CAN 芯片顶标补拍与关闭证据](../evidence/hardware/H0/2026-10-02/README.md)
- [2026-09-17：硬件到货阶段交接与基线汇总报告](Hardware_Arrival_Transition_Report_2026-09-17.md)
- [2026-09-16：双节点外设基线配置与编译验证记录](Dual_Node_Peripheral_Baseline_Verification_Record_2026-09-16.md)
- [2026-09-13：双 ECU 外设资源与低仪器依赖验证草案](Peripheral_Pin_Clock_Interrupt_Allocation_v0.1_Working_Draft_2026-09-13.md)
- [2026-09-12：开发环境搭建与最小编译验证记录](Development_Environment_Verification_Record_2026-09-12.md)

> Gate H2 采用“阶段内本地累计、Gate 完成后统一推送”策略。本次远端更新汇总 2026-10-02～10-07 的连续推进，各记录保留真实发生日期；失败、修复和验收不被压缩成同一天的一次操作。
