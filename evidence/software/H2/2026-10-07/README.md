# Gate H2 实板 CAN 收发、故障注入与恢复证据索引（2026-10-07）

## 1. 结论与时间说明

Gate H2 的低压双节点 CAN 验收结论为 **PASS**。

本目录是一次性 Git 推送中的 `2026-10-07` 运行证据，不覆盖此前过程：

- `2026-10-02`：收发器身份与无源总线起点；
- `2026-10-03`：排针焊接、Node A 供电与初始接线；
- `2026-10-04`：Node B 供电、USB 稳定性与设备身份；
- `2026-10-05`：最小固件构建、刷写及首次运行失败；
- `2026-10-07`：多故障定位、修复、固定帧重复性、正式帧、断线注入和恢复验收。

因此，本次统一推送表达的是一个跨日期推进过程，而不是一次操作全部成功。早期失败日志、恢复失败日志和最终通过日志全部保留。

## 2. 最终受控配置

- Node A/Node B：NUCLEO-F103RB，USB 低压供电；全部 `12 V` 与功率外设断开。
- CAN Pal：TJA1051T/3；`Vcc=3.3 V`、共地、`SLNT` 明确接 GND。
- `PA12/CN10-12 → TX`，`PA11/CN10-14 ← RX`。
- `CANH↔CANH`、`CANL↔CANL`、`GND↔GND`；两端 termination=`ON`。
- 最终恢复前全断电，从两端测得 `CANH-CANL=60.3 Ω / 60.4 Ω`。
- 时钟：HSE bypass 8 MHz、PLL×8、SYSCLK 64 MHz；CAN 500 kbit/s、采样点 87.5%。
- 正式帧：Node B→A 为 `0x100/DLC2`；Node A→B 为 `0x180/DLC5`；周期 100 ms；4 位 AliveCounter。

## 3. 固件构建与刷写基线

| 节点 | text | data | bss | ELF 字节数 | SHA-256 |
|---|---:|---:|---:|---:|---|
| Node A | 33128 | 96 | 14008 | 1096348 | `22A9A6A483E01B15A0307C17CF2E761EEA344352A817B3EBF8D50063C7C8E502` |
| Node B | 29812 | 96 | 13824 | 1078912 | `9FFF03497A93D5F71CEE37C17F38EA42DDA726DA7FE3180AEC82807E09FCF31A` |

两工程使用 STM32CubeIDE 2.2.0 所带 GNU Tools for STM32 14.3.1 构建，退出码均为 0。按板卡身份分别刷写，CubeProgrammer 均返回下载、校验和复位成功。`Debug` 产物按 `.gitignore` 不进入仓库；哈希用于绑定本次实板日志与实际刷入镜像。

## 4. 运行结果

下表的 `tx/rx` 为每份日志最后一条 `STAT`；“四项错误”依次指 `tx_fail/rx_bad/seq_err/q_drop`。TEC/REC 从 bxCAN ESR 的 bit 16–23/24–31 提取；Warning/Passive/Bus-off 分别由 EWGF/EPVF/BOFF 判断。

| 阶段 | 窗口 | Node A | Node B | 判定 |
|---|---:|---|---|---|
| 固定帧、SLNT 修复前 run1 | 80 s | `637/635`，seq_err=1，max TEC=253，Warning/Passive | `640/642`，seq_err=1，max REC=90 | FAIL，保留为对照 |
| 固定帧、SLNT 修复前 run2 | 80 s | `687/688`，max TEC=251，Warning/Passive | `690/690`，seq_err=1，max REC=126，Warning | FAIL，证明“能收发”不等于稳定 |
| 固定帧、SLNT 修复后 run1 | 80 s | `687/688`，四项错误 0 | `690/689`，四项错误 0 | PASS |
| 固定帧、SLNT 修复后 run2 | 80 s | `687/688`，四项错误 0 | `690/689`，四项错误 0 | PASS |
| 固定帧、SLNT 修复后 run3 | 80 s | `687/688`，四项错误 0 | `690/689`，四项错误 0 | PASS |
| 正式 `0x100/0x180` 基线 | 80 s | `687/688`，max TEC/REC=`16/0`，末态 ESR=0 | `690/689`，max TEC/REC=`4/0`，末态 ESR=0 | PASS |
| H/L 受控断开 | 30 s | `rx=0`、tx_fail=194、max TEC=248、Warning/Passive | `rx=0`、tx_fail=187、max TEC/REC=`240/123`、Warning/Passive/Bus-off | 故障明确可观测 |
| 首次恢复 run1 | 80 s | 收发恢复但 max TEC=252、Warning/Passive 持续出现 | seq_err=1 | FAIL，拒绝关门 |
| 无触碰复测 run2 | 30 s | max TEC=208、Warning/Passive | seq_err=1 | FAIL，拒绝关门 |
| 断电复核、两端 `60.3/60.4 Ω` 并重查线束后 run3 | 30 s | `197/198`，四项错误 0，max TEC=3 | `190/189`，四项错误 0，max TEC/REC=0/0 | PASS 预检 |
| 最终恢复 run4 | 80 s | `687/688`，四项错误 0，max TEC/REC=`6/0`，末态 ESR=0 | `690/689`，四项错误 0，max TEC/REC=`6/0`，末态 ESR=0 | **PASS** |

最终恢复窗口内没有任何 EWGF、EPVF 或 BOFF 采样，双方 `hal_err=0`，发送邮箱持续释放。低位 ESR 瞬态只对应不超过 6 的 TEC，不构成 Warning 门槛。

## 5. 原始日志与 SHA-256

| 文件 | SHA-256 |
|---|---|
| `H2_CAN_fixed-frame_pre-SLNT-fix_run1_Node-A_20261007.txt` | `2F9485B885E9BA73AD6EF9C4D58397BEC01925990925F777FED0367ACADB365B` |
| `H2_CAN_fixed-frame_pre-SLNT-fix_run1_Node-B_20261007.txt` | `73863296D8253B554AC51467DEFA80F9B7DB8D7DDCCE2656F3DDBCA68950D693` |
| `H2_CAN_fixed-frame_pre-SLNT-fix_run2_Node-A_20261007.txt` | `39A39F8E8DD8CA1026E57EDDA533361D9C8DD19F997372D33E56DEE3750A3A35` |
| `H2_CAN_fixed-frame_pre-SLNT-fix_run2_Node-B_20261007.txt` | `FBAB993138DDB7B2A5E9F16346DFF1F501A817C4D360B1FE93AB0F42E094E607` |
| `H2_CAN_fixed-frame_post-SLNT-fix_run1_Node-A_20261007.txt` | `46A565274052F06803EE152DDF7541462C90F2C9C06BDB48803D5C227E6D75C6` |
| `H2_CAN_fixed-frame_post-SLNT-fix_run1_Node-B_20261007.txt` | `AFBBC234B1687079270FA814E86627FAF44D27394F5147641D426F6B1081E4C9` |
| `H2_CAN_fixed-frame_post-SLNT-fix_run2_Node-A_20261007.txt` | `62284C4B27A8047F5834DA06BD11120CE39C31B24AD63786ABE98591B1060088` |
| `H2_CAN_fixed-frame_post-SLNT-fix_run2_Node-B_20261007.txt` | `F4CF953CF60E82C53B2FEAA587D52D7815602D5890CC493544364350C4B90FCD` |
| `H2_CAN_fixed-frame_post-SLNT-fix_run3_Node-A_20261007.txt` | `A4944858551F6B70BD743A58B7399C608CB07D45DC575F2E57962DE6FEE00FF8` |
| `H2_CAN_fixed-frame_post-SLNT-fix_run3_Node-B_20261007.txt` | `B7F6C54000984C67815F45D55312B76536128053F28B8ED155016C5EB784B9DA` |
| `H2_CAN_formal-baseline_Node-A_20261007_run01.txt` | `DB205523F209322EA6090407CE39A3A9CCFD6F6A75B27D2B05874B5F8142E822` |
| `H2_CAN_formal-baseline_Node-B_20261007_run01.txt` | `AC943CE91C6F7B0A148CF157F20110FBAC64D77403CB3A18754AA53D11B1567A` |
| `H2_CAN_controlled-HL-disconnect_Node-A_20261007_run01.txt` | `616951DB94CABCC7B48CE8E8485D3C8A370445030DE467CF4C650B1F5F72F1AB` |
| `H2_CAN_controlled-HL-disconnect_Node-B_20261007_run01.txt` | `B046BED2950BF99337CDEDD5BE83A38885975C145E190C0B7443A41E0F4B269C` |
| `H2_CAN_recovery-failed_Node-A_20261007_run01.txt` | `0142A483EA3349F2635FA15A3C06F887DD77DA92AE496F3653AEFDBEEAC396C7` |
| `H2_CAN_recovery-failed_Node-B_20261007_run01.txt` | `9123A5ABC3F6E2A947E4902997EFA58CD89446703960ED6042F2305E34C42736` |
| `H2_CAN_recovery-precheck-failed_Node-A_20261007_run02.txt` | `36DB4F966288E8BD67BE66F0F0147612E4C0E8818BECCA7E2ED5613441F520CF` |
| `H2_CAN_recovery-precheck-failed_Node-B_20261007_run02.txt` | `6E1722EF030421D962DB2CCBC5C6C5555BBFDE72EB209F1E08D28B5495DF0FAB` |
| `H2_CAN_recovery-precheck-pass_Node-A_20261007_run03.txt` | `ECB0EAE9E59CB936E124EBC620CEE2F1025F31D5DE4EEB07225EB21F26859727` |
| `H2_CAN_recovery-precheck-pass_Node-B_20261007_run03.txt` | `A2550C25DBD55B6FAFF277C43169E105F675B4E7431CF73312482E49F2F306AD` |
| `H2_CAN_recovery-final-pass_Node-A_20261007_run04.txt` | `9B06682CD4F2955EC4242A8B19D53E73BD4FF793879175D80E6EA1A43E1BB313` |
| `H2_CAN_recovery-final-pass_Node-B_20261007_run04.txt` | `5B908CAF8584149A30917ED592C1132EA8BCA1AFC1CC39F902B33BB3B40B493E` |

## 6. 判定边界

这些日志可以证明：固定帧与正式帧双向收发、AliveCounter 连续性、断开 H/L 的错误可观察性、以及恢复后的重复稳定窗口。

这些日志不能证明：完整 `300 ms` 应用超时状态机、两次异常确认/三次有效恢复、12 V 功率链、负载、长时间耐久、EMC 或生产级可靠性。完整排障因果链见 [`Gate H2 CAN Bring-up 排障证据复盘`](../../../../current/H2_CAN_Bringup_Troubleshooting_Evidence_Recap_2026-10-07.md)。
