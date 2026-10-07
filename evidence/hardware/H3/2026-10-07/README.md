# Gate H3 硬件证据索引（2026-10-07）

## 当前状态

Gate H3 于 2026-10-07 开始。本目录当前对应 **H3-00 准入与设计冻结**；该步骤只审计现有 H0/H2 证据、固件配置和设计边界，没有执行新的接线、通电、刷写或测量，因此目前没有新增原始照片或运行日志。

H3 总体状态为 **IN PROGRESS**，H3-00 已 PASS，H3-01 尚未开始。后续统一推送时仍保留每个动作的真实发生日期，不把阶段关闭日期回填为本目录内所有工作的发生日期。

## H3-00 可追溯输入

| 输入 | 已采用事实 | 限制 |
|---|---|---|
| [Gate H0 硬件验收基线](../../../../current/05_verification_gates_验证与门禁/Gate_H0_Hardware_Acceptance_Baseline_H0硬件验收基线_2026-09-22.md) | NTC-01=`10.30 kΩ`；R10K-01=`9.911 kΩ` | NTC 单点阻值不能证明 B3950 |
| [Gate H2 CAN 物理层执行记录](../../../../current/05_verification_gates_验证与门禁/Gate_H2_CAN_Physical_Layer_Execution_Record_H2_CAN物理层执行记录_2026-10-02.md) | H2 PASS；关闭提交 `bc19580` 可作为固件回退基线 | 不证明 NTC/ADC 或任何 12 V 外设 |
| [外设引脚时钟中断分配](../../../../current/03_hardware_interfaces_硬件与接口/Peripheral_Pin_Clock_Interrupt_Allocation_外设引脚时钟中断分配_v0.1_Working_Draft_工作草案_2026-09-13.md) | Node B NTC=`ADC1_IN0 / PA0 / A0 / CN7-28`；100 ms；上电校准 | 开短路阈值仍需 H3 实测冻结 |
| Node B `node_b.ioc`、`Src/adc.c` | 单通道、软件触发、右对齐、55.5 cycles、ADC clock≈10.67 MHz | 只有初始化骨架，尚无校准/采样/换算业务代码 |

## 本日输出

- [Gate H3 单外设验证执行记录](../../../../current/05_verification_gates_验证与门禁/Gate_H3_Single_Peripheral_Validation_Execution_Record_H3单外设验证执行记录_2026-10-07.md)
- 冻结拓扑：`3V3 → NTC-01 → A0/PA0 → R10K-01(9.911 kΩ) → GND`；
- 冻结趋势：升温时 ADC raw 上升；
- 冻结边界：B3950 为采购标称而非单点实测结论；12 V、Fan、Heater、INA260 继续禁止接入。

## 待归档证据

H3-01 开始后，再按实际日期归档板外分压接线照片、无源阻值、Node B A0 接线照片和后续 UART 原始日志。尚未执行的项目不创建占位“通过”证据。
