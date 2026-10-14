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
| [`H3_NODE-A_TACH-00E_routing-separated-peakdetect_20261014_01.jpg`](H3_NODE-A_TACH-00E_routing-separated-peakdetect_20261014_01.jpg) | `199563` bytes | `0CA20BB88210244755D13C85BAC180508B65383605F9932247074058D488C1AA` | 外露布线分离后，5 ms/div 下真实 43.18 Hz Tach 与密集 25 kHz Peak Detect 包络并存 |
| [`H3_NODE-A_TACH-00E_routing-separated-pwm-coupling_20261014_02.jpg`](H3_NODE-A_TACH-00E_routing-separated-pwm-coupling_20261014_02.jpg) | `186884` bytes | `5EBE0DE8166172ABF3F1EEDB791A7921B15A08F22E59948716AE833906A16C28` | 25 us/div 下 24.98 kHz / 40.02 us 尖脉冲及额外阈值穿越 |
| [`H3_NODE-A_TACH-00E_routing-separation-ineffective_20261014.txt`](H3_NODE-A_TACH-00E_routing-separation-ineffective_20261014.txt) | `3433` bytes | `54406DEC36D5859343EFC12BD7ABCCE4FD3C944AFA846AEA8E35BD18C3CC57A8` | 照片代填读数、最差极值、1.6 V 阈值判读、布线措施无效结论与 RC 候选拓扑 |
| [`H3_NODE-A_TACH-00F_RC-filtered-waveform_20261014_01.jpg`](H3_NODE-A_TACH-00F_RC-filtered-waveform_20261014_01.jpg) | `189594` bytes | `B54DCEB70C7F5D2FE67C7DA4C6363FC70FFB5A7A5A1F032C3C4B1E51969978BA` | `10 kΩ + 10 nF` 滤波点的 47.97 Hz / 20.85 ms 干净方波及完整逻辑摆幅 |
| [`H3_NODE-A_TACH-00F_RC-filter-pass_20261014.txt`](H3_NODE-A_TACH-00F_RC-filter-pass_20261014.txt) | `3591` bytes | `5D27B596B62BDC784515B82BCD06C62E3E17753767594E0BE248EFF7EAB913D6` | 无源检查、口述/照片读数边界、1.6 V 阈值解释、RC PASS 与 PA0 接入前的剩余门禁 |
| [`H3_NODE-A_TACH-00G_PA0-loaded-waveform_20261014_01.jpg`](H3_NODE-A_TACH-00G_PA0-loaded-waveform_20261014_01.jpg) | `216544` bytes | `07F7A090C64C9562ADD64E558A8815013C5CA77299CE5A3A071DBCE9C04CE7EE` | PA0 接入后的 45.77 Hz / 21.85 ms 完整 Tach 方波与 -80 mV～3.44 V 逻辑摆幅 |
| [`H3_NODE-A_TACH-00G_PA0-loaded-edge_20261014_02.jpg`](H3_NODE-A_TACH-00G_PA0-loaded-edge_20261014_02.jpg) | `181949` bytes | `042B616F6C548783D325B1CC6C804F3C575787D401B4F213C794C755E090FFE8` | 25 us/div 下单调 RC 上升沿，无可见 25 kHz 凹口或额外 1.6 V 穿越 |
| [`H3_NODE-A_TACH-00G_PA0-load-pass_20261014.txt`](H3_NODE-A_TACH-00G_PA0-load-pass_20261014.txt) | `4169` bytes | `7476C053D4B7A5A2B81F3C2E958107055ED89D21C16C1B48BE88063D2C5AB27E` | PA0 接入、在路方向性阻值解释、两张照片判读、TACH-00G PASS 及固件捕获前边界 |

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
- TACH-00E 在恢复 F4 后尝试分离外露 F3/F4 跳线和探测回路，但仍测得
  `24.98 kHz / 40.02 us` 尖脉冲、最差 `3.72 V / -2.16 V`，高平台负向尖脉冲会
  额外穿越 `1.6 V`；真实 Tach 为 `43.18 Hz / 23.16 ms`；
- 外部布线分离判定为 **NOT EFFECTIVE**，不再继续反复调线；PA0/A0 仍未放行，下一步
  转入可拆卸 RC 滤波测试；
- TACH-00F 新增 `10 kΩ series + 10 nF to GND`，实测串联电阻 `9.997 kΩ`；正常 F4
  接法下 25 kHz 残余消失，完整周期照片为 `3.40 V / -40 mV / 47.97 Hz /
  20.85 ms`，跨采集状态最差负向极值为 `-120 mV`；
- 滤波后每个真实边沿只穿越 1.6 V 一次，TACH-00F 判定为 **PASS（PA0 隔离的电气
  滤波范围）**；
- TACH-00G 将滤波点接入 CN8/A0（PA0）后，静态电压为 `3.234 V`，运行波形为
  `45.77 Hz / 21.85 ms / 3.44 V / -80 mV`，Peak Detect 最差负向极值 `-120 mV`；
- 25 us/div 下只看到单调 RC 边沿，无可见 25 kHz 残余或额外 1.6 V 穿越；TACH-00G
  判定为 **PASS（PA0 高阻输入负载与短时电气波形范围）**。Tach 接入 MCU 前的低压
  电气门禁已关闭，下一步允许受控启动 TIM2 捕获；PPR 与 RPM 仍为 TBD。

