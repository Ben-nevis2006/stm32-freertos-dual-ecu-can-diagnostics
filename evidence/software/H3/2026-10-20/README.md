# H3 Tach 输入捕获软件证据（2026-10-20）

## 1. 范围与时间边界

本目录承接 2026-10-19 的 TACH-01B 实板周期完整性失败，记录 2026-10-20 构建的
TACH-01C。两天的记录按真实推进顺序分目录保留；不把构建日期回填成上一轮日期。

## 2. 文件与完整性

| 文件 | 大小 | SHA-256 | 内容 |
|---|---:|---|---|
| [`H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt`](H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt) | `4406` bytes | `B9EB7E1913A9BF86C524C185612A7A64EE00279978E92DCF2190651C9AAF8C53` | CKD 单变量、CubeMX 同步、clean build、ELF 身份、静态交叉核验与实板门禁 |
| [`H3_NODE-A_TACH-01C_filter15-ckd4-runtime_20261020.txt`](H3_NODE-A_TACH-01C_filter15-ckd4-runtime_20261020.txt) | `7108` bytes | `AE025C503C0C874912DF1A0CCAA65934B402F4ECC7A71F2AC06DC9E17454AF37` | 定向刷写、可观察启动、15 窗口零边沿基线、25 窗口实风扇周期完整性验收 |

## 3. 当前结论

- TACH-01C 只把 TIM2 CKD 从 DIV1 改为 DIV4；`ICFilter=15` 和其余采集条件不变；
- 计数器仍为 `100 kHz / 10 us`，滤波值 15 的名义连续稳定时间由约 4 us 延长至 16 us；
- clean build、ELF 静态核验、定向刷写、下载校验和启动身份回读均通过；
- 无风扇 15 个窗口/14.448 s 保持零边沿；
- 默认 50% 实风扇 25 个连续窗口均为完整 `23.33～23.75 ms` 周期，平均
  `42.472598 edge/s`，没有半周期、微秒级短周期或 zero_delta；
- `TACH-01C` 判定为 **PASS（原始 Tach 捕获完整性）**；
- PPR/RPM 继续为 `TBD / INVALID`，绝对转速与故障阈值必须由后续独立门禁关闭。
