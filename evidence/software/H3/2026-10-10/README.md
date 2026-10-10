# H3 软件证据：2026-10-10

本目录保存 Node B NTC H3-04 边界分类实现、离线构建、定向刷写以及新固件下的正常—开路—短路—恢复实测。2026-10-09 先完成旧固件样本表征，2026-10-10 才实现并验收三态；统一提交或后续统一推送不得抹去该时间差。

## 实现与阈值来源

Node B `freertos.c` 新增：

| ADC raw | `ntc_state` | `calc_valid` | 含义 |
|---:|---:|---:|---|
| `0～50` | `1` | `0` | NTC 上支路开路（OPEN） |
| `51～3949` | `0` | `1` | 当前 H3 实验正常区间（NORMAL） |
| `3950～4095` | `2` | `0` | NTC 被旁路/近满量程（SHORT） |

阈值来自[2026-10-09 旧固件表征](../2026-10-09/README.md)：开路当前值最大 `17`、窗口最大 `43`；旁路当前值最小 `3990`、窗口最小 `3973`。边界状态不再输出看似有效的极大/极小阻值，而固定 `r_ntc_ohm=0 / calc_valid=0`。完整决策、统计和边界见 [`H3_NODE-B_NTC_boundary-summary_20261010.txt`](H3_NODE-B_NTC_boundary-summary_20261010.txt)。

## 构建与定向刷写

| 项目 | 结果 |
|---|---|
| 工程 / 工具链 | `firmware/node_b`；STM32CubeIDE 2.2.0；GNU Tools for STM32 14.3.1 |
| 构建 | 退出码 `0`；warnings/errors=`0/0` |
| `text / data / bss / dec` | `33780 / 100 / 13844 / 47724` bytes |
| ELF | `1095288` bytes；SHA-256=`8CFC084B8AD03683931CC8FDF91C4D8BCF46173FD88F2EA63D006BE6A364A9A2` |
| 目标 | 唯一连接的冻结 Node B ST-LINK；`0x410 / 128 KiB / Cortex-M3`；目标电压 `3.24 V` |
| 写入结果 | `Download verified successfully`，随后请求 reset |
| Node A | 未连接、未访问 |

规范化记录见 [`H3_NODE-B_NTC_boundary-build_20261010.txt`](H3_NODE-B_NTC_boundary-build_20261010.txt) 与 [`H3_NODE-B_NTC_boundary-flash-verify_20261010.txt`](H3_NODE-B_NTC_boundary-flash-verify_20261010.txt)。它们明确是受控摘要，不冒充未保存的完整工具控制台转储。

## 启动绑定与正常基线

`15:51:55.184+08:00` 的实板启动记录为：

```text
H3,B,NTC_BOOT,adc=ADC1_IN0,sample_ms=100,log_ms=1000,vdda_cfg_mv=3300,r_fixed_ohm=9911,open_raw_max=50,short_raw_min=3950,cal=0,ready=1
```

它把运行固件的阈值、ADC 校准和就绪状态绑定到本次串口证据。此前 `15:42:57～15:44:07` 的正常接线采集包含 68 条 H3：raw=`1786～1867`、均值 `1827.03`，68/68 为 `state=0 / calc_valid=1`，`fail=0 / last=0`。

## 新固件边界与恢复验收

| 实际时间 | 状态 | H3 条数 / 跨度 | 当前 raw | 状态完整性 | 判定 |
|---|---|---:|---:|---|---|
| `16:03:23～16:03:45` | 断开上端 `T` | `21 / 20.499 s` | `0～15`，均值 `1.05` | 21/21 `state=1 / valid=0 / r=0`；`fail=0` | OPEN PASS |
| `16:08:34～16:08:54` | `T-S` 旁路 NTC | `21 / 20.534 s` | `3961～4095`，均值 `4064.19` | 21/21 `state=2 / valid=0 / r=0`；`fail=0` | SHORT PASS |
| `16:12:15～16:12:36` | 恢复原始接线 | `21 / 20.536 s` | `1878～1934`，均值 `1911.43` | 21/21 `state=0 / valid=1`；`fail=0` | RECOVERY PASS |

三段 `samples` 均按每条约 10 次采样推进，`last=0`，未见 ADC 失败或状态卡死。据此，H3-04 于 **2026-10-10 PASS**。

同一原始日志中的 `H2,B,STAT` 发送失败来自本次 Node B 单板、无 CAN 对端的 H3 上下文，不是 ADC 或 NTC 分类失败，也不能用来重判或替代已关闭的 H2 双节点验收。

## H3-05 NTC 子项证据包结论

NTC 子项已经具备以下可回溯链：

1. 2026-10-07～10-08：板外阻值、近照、线色规范化、连续性、上电前短路检查及 DMM 电压；
2. 2026-10-08：ADC 实现、构建、Node B 定向刷写、运行冒烟、室温与 Run01；
3. 2026-10-09：自行计时 Run02、恢复延长段、旧固件开/短路表征及在路阻值排查；
4. 2026-10-10：阈值实现、构建/刷写绑定、新固件 OPEN/SHORT/RECOVERY 验收；
5. 当前有效执行记录与独立[H3 NTC 联调排障复盘](../../../../current/06_engineering_recaps_工程复盘/H3_NTC_Bringup_Troubleshooting_Evidence_Recap_H3_NTC联调排障证据复盘_2026-10-10.md)。

据此，H3-05（关闭 NTC 子项并形成独立证据包）于 **2026-10-10 PASS**。整个 Gate H3 仍为 **IN PROGRESS**：Fan PWM、Tach/PPR 和 INA260 尚未完成各自准入与验证，不能借用 NTC 结果代替。

## 必须保留的工程边界

- 当前阈值是本台架的 H3 实验识别线，不是生产全温压诊断线；新固件 SHORT 最低当前值 `3961` 距 `3950` 仅 11 count。
- 尚无滞回、去抖、持续时间、锁存、DTC/CAN 故障上报或合法温度区间。
- 未以参考温度计完成多点标定，不能宣称 B3950 或绝对温度精度。
- 未覆盖长期稳定、机械振动、器件容差、VDDA 漂移、ADC 误差、老化、EMC/ESD。
- `T-S` 旁路由约 9.93 kΩ 下臂限流；禁止把本流程机械套用于没有限流路径的节点。

## 证据文件完整性

| 文件 | 字节 | SHA-256 |
|---|---:|---|
| `H3_NODE-B_NTC_boundary-build_20261010.txt` | `1800` | `8D5DFDAB37D5397CADBBA6A8105CB2658AF311B8D91C12E0FC9ADDC0B2F0A53D` |
| `H3_NODE-B_NTC_boundary-flash-verify_20261010.txt` | `1427` | `22E3496C096AF8D407ACBFBBB849A4A75E2F16C38021CD9CCD84C90874D02DCC` |
| `H3_NODE-B_NTC_boundary-summary_20261010.txt` | `3700` | `0F085B46505B6B78C4409A6CB5A58E9EC311ABC7DAF299956B3F49EE7C4A74F7` |
| `H3_NODE-B_NTC_threshold-normal_20261010_run01.txt` | `25343` | `DC3AFC386654428ADC96C4606B090F8BE79FF98EF5FEB78A2B2CC081E362BB28` |
| `H3_NODE-B_NTC_threshold-boot-normal_20261010_run01.txt` | `9400` | `9646B8DBFD6E0501C141CF7BD300B53B2A606AC9B651674D7C617756FA15896B` |
| `H3_NODE-B_NTC_threshold-open_20261010_run01.txt` | `7783` | `AD8BF7FB3DE925063608D3CB3D14D733F6724E8FC24D9D3110389C4C9B3592A8` |
| `H3_NODE-B_NTC_threshold-short_20261010_run01.txt` | `7935` | `00779F5516036E4A2FC22D5A85C9B752B556CE87802C85C860326D0D1BF181FA` |
| `H3_NODE-B_NTC_threshold-final-recovery_20261010_run01.txt` | `8116` | `DA785CF67709688EABA24CB4DE2A4B3B4F289E53F9CEBD7831BEAF61140E2542` |

哈希绑定本次归档字节。若后续修改任何对应文本，必须重新计算并同步本表；README 自身不列入自身哈希，避免循环依赖。
