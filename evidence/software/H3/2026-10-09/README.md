# H3 软件证据：2026-10-09

本目录保存 H3-03 Node B NTC 重复性 Run02 的原始串口日志、恢复延长段和统计摘要。所有时间均为 Asia/Shanghai；文件名与正文保留实际日期，不与 2026-10-08 的基线和 Run01 合并伪装为同日操作。

## Run02：自行计时 30 秒加温—自然恢复

执行人只以手指接触封装后的 NTC 珠体，并自行计时 30 秒后主动松手；未使用通电加热器、热风、烙铁或 12 V。主采集在打开 COM8 后清空主机旧缓存，保留 351 条连续 H3 报告；随后追加 88 条完全不触碰的恢复尾段。

| 指标 | 结果 | 判定 |
|---|---:|---|
| 主采集跨度 | 主机 `359.087 s`；设备 tick `359.100 s` | 连续 |
| H3 / `samples` / `fail` | `351`；`480→3980`；`0→0` | PASS |
| 相邻 tick / samples 增量 | 全部 `1026 ms / 10` | PASS |
| 加温前配对 30 条均值 | raw=`1830.867`、`1475.033 mV nominal`、`12256.833 Ω` | 记录凌晨环境工作点 |
| 首次 10 条 rolling median 越阈 | `1949.5`，冻结阈值 `1921` | PASS |
| 最大 rolling median | `2396` | 持续响应，不是单点尖峰 |
| 峰值 | raw=`2415`、`1946 mV nominal`、`6894 Ω` | raw 上升、阻值下降，方向正确 |
| 首个恢复 30 条窗口 | `00:43:47–00:44:16`，raw 均值 `1889.667` | 落入冻结区间 `1839～1891`，PASS |
| 恢复延长段 | `88` 条；最终 30 条 raw 均值 `1825.767` | 相对本轮加温前配对均值仅 `-5.100`（`-0.279%`） |
| 状态字段 | 全部 `cal=0 / ready=1 / calc_valid=1 / fail=0 / last=0` | PASS |

Run02 再次观察到升温时 raw/名义电压上升、反算 NTC 阻值下降，且自然恢复后完整 30 条均值进入预先冻结的区间，因此与 Run01 共同满足“趋势可重复”。H3-03 于 2026-10-09 判定 **PASS**。

必须保留的边界：30 秒接触时长来自执行人自行计时，不是独立接触传感器测得；凌晨的未加温工作点低于 2026-10-08 晚间基线，恢复延长段也稳定在较低工作点。该变化按环境/热平衡语境记录，不能据此声称绝对温度精度，也不能删除扩展段中低于旧绝对包络的数据。一次在 2026-10-08 发生的初步 Run02 因助手额度暂停而缺失完整恢复段，已排除出 PASS 证据，不解释为硬件故障。

逐行记录见 [`H3_NODE-B_NTC_warm-recovery_20261009_run02.txt`](H3_NODE-B_NTC_warm-recovery_20261009_run02.txt)；未触碰恢复尾段见 [`H3_NODE-B_NTC_warm-recovery_20261009_run02_recovery-extension.txt`](H3_NODE-B_NTC_warm-recovery_20261009_run02_recovery-extension.txt)；受控统计及逐项判据见 [`H3_NODE-B_NTC_warm-recovery_20261009_run02_summary.txt`](H3_NODE-B_NTC_warm-recovery_20261009_run02_summary.txt)。

## 证据文件完整性

| 文件 | SHA-256 |
|---|---|
| `H3_NODE-B_NTC_warm-recovery_20261009_run02.txt` | `A1F4B49A063ECE1396CB5D5FC4A8A0C90DB29382E35152E67C561550F5543FE0` |
| `H3_NODE-B_NTC_warm-recovery_20261009_run02_recovery-extension.txt` | `2AF6A6A7238E7165B1B7DF0DE8A30D0938BAE5AFDE37CF9A6375F3B37E7C685F` |
| `H3_NODE-B_NTC_warm-recovery_20261009_run02_summary.txt` | `916CF23C45D146B92C367603734579A2E7AFC07CBF89201DA87D6F143028A31B` |

以上哈希绑定仓库归档字节。原始串口记录未删点或平滑；归档文本采用仓库换行规范，因此临时采集文件与仓库文件的字节哈希不混用。
