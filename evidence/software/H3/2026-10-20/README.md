# H3 Node A Tach 与 INA260 软件证据（2026-10-20）

## 1. 范围与时间边界

本目录承接 2026-10-19 的 TACH-01B 实板周期完整性失败，记录 2026-10-20 构建和实测的
TACH-01C、随后不改线不刷写完成的 TACH-02A PPR/全档位量程、据此构建并通过实板验证的
TACH-02B RPM/新鲜度镜像，以及 Tach 子项关闭后准备的 INA-01 只读扫描/身份/采样镜像。
不同动作均按真实推进顺序记录；不把构建日期回填成上一轮日期，也不把 INA-01 离线构建
写成实板功能通过。

## 2. 文件与完整性

| 文件 | 大小 | SHA-256 | 内容 |
|---|---:|---|---|
| [`H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt`](H3_NODE-A_TACH-01C_filter15-ckd4-build_20261020.txt) | `4406` bytes | `B9EB7E1913A9BF86C524C185612A7A64EE00279978E92DCF2190651C9AAF8C53` | CKD 单变量、CubeMX 同步、clean build、ELF 身份、静态交叉核验与实板门禁 |
| [`H3_NODE-A_TACH-01C_filter15-ckd4-runtime_20261020.txt`](H3_NODE-A_TACH-01C_filter15-ckd4-runtime_20261020.txt) | `7108` bytes | `AE025C503C0C874912DF1A0CCAA65934B402F4ECC7A71F2AC06DC9E17454AF37` | 定向刷写、可观察启动、15 窗口零边沿基线、25 窗口实风扇周期完整性验收 |
| [`H3_NODE-A_TACH-02A_ppr2-duty-range-runtime_20261020.txt`](H3_NODE-A_TACH-02A_ppr2-duty-range-runtime_20261020.txt) | `5997` bytes | `B00F692890159DF88836FF9CDFCC9E1EA4950A1CDF22BC3AB9256104CD07D6B7` | PPR 一手来源与实物交叉证明、五档 Tach/RPM 量程、停转/重启及 stale 语义缺口 |
| [`H3_NODE-A_TACH-02B_ppr2-rpm-stale-build_20261020.txt`](H3_NODE-A_TACH-02B_ppr2-rpm-stale-build_20261020.txt) | `6251` bytes | `423DE7A2D22172763AD8BBBAE94D7456DFBFFFE2A96CDB7A00EFA1F040649A05` | PPR=2 整数 RPM、500 ms 新鲜度状态、clean build、ELF 身份、栈/长度/CAN 占位交叉核验与实板门禁 |
| [`H3_NODE-A_TACH-02B_ppr2-rpm-stale-runtime_20261020.txt`](H3_NODE-A_TACH-02B_ppr2-rpm-stale-runtime_20261020.txt) | `7348` bytes | `EE90AE89A710FDEAD4A3540A7072479ADF853E76FB5711CB67EC34DE3E1BC82A` | 定向刷写、无风扇 INVALID 基线、默认 50% 数值 RPM、全档位 STALE/恢复语义及长时 COM7 证据 |
| [`H3_NODE-A_INA-01_read-only-scan-build_20261020.txt`](H3_NODE-A_INA-01_read-only-scan-build_20261020.txt) | `5232` bytes | `026D656458F5E4736BEF462F2D9D8CC8D30B41072CAB395243575BFA7335BEA0` | INA260 全地址只读扫描、寄存器身份、整数工程量采样、clean build、ELF/源码哈希、只读 API/栈/长度交叉核验及实板门禁 |

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
  至此 Tach 子项关闭。CAN RPM/故障位正常链路留给 H5，Stall/断线等 Fault/Recovery
  留给 H6，不作为 H3 阻塞项；H3 当前仅余 INA260 独立验证。
- INA-01 已在基线 `e92ea2f` 上完成只读实现和 clean build；最终 ELF SHA-256 为
  `835C48D09500657D3FA38F623B631C04B98A37B0B7B84D8C5197C71602CA453B`，只含设备就绪
  探测和寄存器读取，不含任何 I2C 写路径；
- `INA-01` 当前判定为 **PASS（离线实现/构建范围）**，硬件形成该证据时仍全断电、模块
  孤立且镜像尚未刷写。I2C 地址/身份、连续空载采样和 12 V 高边测量仍待分步实板验证，
  因此 Gate H3 继续保持 **IN PROGRESS**。
