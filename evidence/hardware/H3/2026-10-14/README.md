# H3 Tach/PPR 硬件证据（2026-10-14）

## 1. 范围

本目录从 Fan PWM 子项关闭后的 Tach/PPR 独立准入开始。首轮仅在 F3 与 MCU A0/PA0
隔离的条件下表征 Tach 输出电平和脉冲，不改固件、不推定 PPR。

## 2. 当前证据

| 文件 | 大小 | SHA-256 | 支持事实 |
|---|---:|---|---|
| [`H3_NODE-A_TACH-00B_isolated-waveform_20261014.txt`](H3_NODE-A_TACH-00B_isolated-waveform_20261014.txt) | `2578` bytes | `A04C5234D6CF04DA9098DCB0C36DBE38107DC975BF5BAD9F627136878E20214D` | 4.7 kΩ 上拉、断电门禁、F3 静态上拉、默认 50% 下的隔离方波、电平/频率/周期、USB 稳定性与边沿观察项 |
| [`H3_NODE-A_TACH-00C_peak-detect-pwm-coupling_20261014_01.jpg`](H3_NODE-A_TACH-00C_peak-detect-pwm-coupling_20261014_01.jpg) | `195399` bytes | `D13FF72FCD11AF190AA96F9ABDB1869FFBF8409FA6A0702E6FBCC756174D8C57` | 25 us/div 下约 25 kHz / 40 us 的重复尖脉冲及 Peak Detect 边沿视图 |
| [`H3_NODE-A_TACH-00C_average16-pwm-coupling_20261014_02.jpg`](H3_NODE-A_TACH-00C_average16-pwm-coupling_20261014_02.jpg) | `196057` bytes | `6BB5B12C2551E039927934CDD61AF744B18F347D3198EB5B8FC2B426A6A3254E` | Average 16 后尖脉冲减弱但仍保持约 25 kHz / 40 us 周期 |
| [`H3_NODE-A_TACH-00C_edge-integrity-hold_20261014.txt`](H3_NODE-A_TACH-00C_edge-integrity-hold_20261014.txt) | `3163` bytes | `E01CEF99C5DA743BF19C76E65740A874F36CD79034219C9AE6CBF085D8BCF9E7` | Peak Detect 极值、照片逐屏判读、PWM 同频识别、PA0 接入 HOLD 与下一隔离矩阵 |
| [`H3_NODE-A_TACH-00D_F4-open-clean-edge_20261014_01.jpg`](H3_NODE-A_TACH-00D_F4-open-clean-edge_20261014_01.jpg) | `177024` bytes | `73CC12641D9D9F46CC3853F9A5811139B7100673B0FFEAE6714610AECAC0F89B` | F4 完全断开后，25 us/div 下无 25 kHz 尖脉冲的单个真实 Tach 上升沿 |
| [`H3_NODE-A_TACH-00D_F4-open-fullspeed-waveform_20261014_02.jpg`](H3_NODE-A_TACH-00D_F4-open-fullspeed-waveform_20261014_02.jpg) | `185822` bytes | `7CB5536F7010CDE91A06580A62E4A41870FFBB472D78BFB390C77666A3C5D9A8` | F4 完全断开、风扇 12 V 上电时 91.58 Hz / 10.92 ms 的连续真实 Tach 方波 |
| [`H3_NODE-A_TACH-00D_F4-source-isolation_20261014.txt`](H3_NODE-A_TACH-00D_F4-source-isolation_20261014.txt) | `3417` bytes | `28E4420438FBA8B6AF767A9E75C32334AAA23BDA937D9FBEC9F84FC1E7F8BF85` | 三态原始报告、两张照片逐屏读数、F4/PWM 来源定位、PASS 边界及 PA0 继续 HOLD 的原因 |

## 3. 当前结论

- `R4K7-01=4.702 kΩ`，与 H0 记录一致；
- F3 在风扇未供 12 V 时稳定为 `3.30 V`；
- 风扇运行后为约 `0.00～3.36 V`、`46.38 Hz / 21.56 ms` 的方波；
- `TACH-00B` 在“隔离电气域与方波存在性”范围判定 PASS；
- 高平台两侧的周期性上翘/下拉闪动保留为 `OBS-TACH-01`，需先做边沿完整性复核；
- A0/PA0 仍未连接，PPR 与 RPM 仍为 TBD。
- TACH-00C 在 25 us/div 下捕获到约 `25 kHz / 40 us` 的重复尖脉冲，与风扇 PWM
  同频，不是约 46 Hz Tach 的真实周期；
- TACH-00C 的 Peak Detect 报告短时极值达到 `3.88 V / -2.16 V`，在该轮结束时来源尚未完成隔离；
- TACH-00C 在来源未知阶段判定为 **HOLD**；
- TACH-00D 在 F4/PWM 完全断开后不再出现 25 kHz 尖脉冲，且风扇 12 V 上电时得到
  `91.58 Hz / 10.92 ms` 的规则方波，因此判定为 **PASS（F4/PWM 来源隔离范围）**；
- `OBS-TACH-01` 已从“未知来源”收敛为“F4/PWM 接通路径造成的耦合”，但具体物理耦合点
  尚未确定；
- F4 已恢复而正常接法尚未加入抑制措施，故 PA0/A0 仍未放行，PPR 与 RPM 仍为 TBD。

