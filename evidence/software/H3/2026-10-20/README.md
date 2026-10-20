# H3 Tach 输入捕获软件证据（2026-10-20）

## 1. 范围与时间边界

本目录承接 2026-10-19 的 TACH-01B 实板周期完整性失败，记录 2026-10-20 构建和实测的
TACH-01C、随后不改线不刷写完成的 TACH-02A PPR/全档位量程，以及据此构建的 TACH-02B
RPM/新鲜度镜像。两天的记录按真实推进顺序分目录保留；不把构建日期回填成上一轮日期。

## 2. 文件与完整性

| 文件 | 大小 | SHA-256 | 内容 |
|---|---:|---|---|
| [`H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt`](H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt) | `4406` bytes | `B9EB7E1913A9BF86C524C185612A7A64EE00279978E92DCF2190651C9AAF8C53` | CKD 单变量、CubeMX 同步、clean build、ELF 身份、静态交叉核验与实板门禁 |
| [`H3_NODE-A_TACH-01C_filter15-ckd4-runtime_20261020.txt`](H3_NODE-A_TACH-01C_filter15-ckd4-runtime_20261020.txt) | `7108` bytes | `AE025C503C0C874912DF1A0CCAA65934B402F4ECC7A71F2AC06DC9E17454AF37` | 定向刷写、可观察启动、15 窗口零边沿基线、25 窗口实风扇周期完整性验收 |
| [`H3_NODE-A_TACH-02A_ppr2-duty-range-runtime_20261020.txt`](H3_NODE-A_TACH-02A_ppr2-duty-range-runtime_20261020.txt) | `5997` bytes | `B00F692890159DF88836FF9CDFCC9E1EA4950A1CDF22BC3AB9256104CD07D6B7` | PPR 一手来源与实物交叉证明、五档 Tach/RPM 量程、停转/重启及 stale 语义缺口 |
| [`H3_NODE-A_TACH-02B_ppr2-rpm-stale-build_20261020.txt`](H3_NODE-A_TACH-02B_ppr2-rpm-stale-build_20261020.txt) | `6251` bytes | `423DE7A2D22172763AD8BBBAE94D7456DFBFFFE2A96CDB7A00EFA1F040649A05` | PPR=2 整数 RPM、500 ms 新鲜度状态、clean build、ELF 身份、栈/长度/CAN 占位交叉核验与实板门禁 |
| [`H3_NODE-A_TACH-02B_ppr2-rpm-stale-runtime_20261020.txt`](H3_NODE-A_TACH-02B_ppr2-rpm-stale-runtime_20261020.txt) | `7348` bytes | `EE90AE89A710FDEAD4A3540A7072479ADF853E76FB5711CB67EC34DE3E1BC82A` | 定向刷写、无风扇 INVALID 基线、默认 50% 数值 RPM、全档位 STALE/恢复语义及长时 COM7 证据 |

## 3. 当前结论

- TACH-01C 只把 TIM2 CKD 从 DIV1 改为 DIV4；`ICFilter=15` 和其余采集条件不变；
- 计数器仍为 `100 kHz / 10 us`，滤波值 15 的名义连续稳定时间由约 4 us 延长至 16 us；
- clean build、ELF 静态核验、定向刷写、下载校验和启动身份回读均通过；
- 无风扇 15 个窗口/14.448 s 保持零边沿；
- 默认 50% 实风扇 25 个连续窗口均为完整 `23.33～23.75 ms` 周期，平均
  `42.472598 edge/s`，没有半周期、微秒级短周期或 zero_delta；
- `TACH-01C` 判定为 **PASS（原始 Tach 捕获完整性）**；
- TACH-02A 以 Intel 4-wire 规范、ARCTIC 额定范围和同一实物全速频率将 PPR 冻结为 2；
- 100/50/25% 稳态约为 `2760 / 1360 / 586 RPM`，0% 停转、再次 100% 重启和最终 50%
  恢复均通过，所有窗口 zero_delta=0；
- `TACH-02A` 判定为 **PASS（PPR=2 + 原始全档位量程）**；
- TACH-02B 已完成 PPR=2 整数 RPM、500 ms 新鲜度、STALE/rpm_valid 语义、clean build、
  冻结 SN 定向刷写和启动身份回读；最终 ELF SHA-256 为
  `7A8B655F02A9F6F6EDA528C1C923BEC495516A09C36480AA775F427EFFA13F1E`；
- 无风扇 15 窗口均为 WAIT_FIRST_EDGE/rpm_valid=0；默认 50% 连续输出数值 RPM；B1 的
  100/50/25/0/100/final50 全序列完整，0% 后 9 个连续窗口为 STALE/rpm=INVALID，
  再次 100% 恢复 PERIOD_VALID，所有窗口 zero_delta=0；
- `TACH-02B` 判定为 **PASS（PPR=2 整数 RPM + 500 ms 数据新鲜度实板语义）**；
  Stall/断线故障阈值、CAN RPM/故障位接入与 INA260 仍待后续独立验证。
