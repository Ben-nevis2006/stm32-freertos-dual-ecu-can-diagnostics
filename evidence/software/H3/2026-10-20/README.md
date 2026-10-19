# H3 Tach 输入捕获软件证据（2026-10-20）

## 1. 范围与时间边界

本目录承接 2026-10-19 的 TACH-01B 实板周期完整性失败，记录 2026-10-20 构建的
TACH-01C。两天的记录按真实推进顺序分目录保留；不把构建日期回填成上一轮日期。

## 2. 文件与完整性

| 文件 | 大小 | SHA-256 | 内容 |
|---|---:|---|---|
| [`H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt`](H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt) | `4406` bytes | `B9EB7E1913A9BF86C524C185612A7A64EE00279978E92DCF2190651C9AAF8C53` | CKD 单变量、CubeMX 同步、clean build、ELF 身份、静态交叉核验与实板门禁 |

## 3. 当前结论

- TACH-01C 只把 TIM2 CKD 从 DIV1 改为 DIV4；`ICFilter=15` 和其余采集条件不变；
- 计数器仍为 `100 kHz / 10 us`，滤波值 15 的名义连续稳定时间由约 4 us 延长至 16 us；
- clean build 与 ELF 静态核验通过，尚未刷写实板；
- 当前 MCU 仍运行 TACH-01B，必须先断风扇 12 V 并确认残压安全后才能定向刷写；
- PPR/RPM 继续为 `TBD / INVALID`，TACH-01C 不因构建成功而提前通过。
