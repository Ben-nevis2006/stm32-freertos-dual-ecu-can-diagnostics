# Gate H3 软件证据索引（2026-10-08）

## 本日范围

本目录记录 H3-02 的 Node B NTC ADC 最小增量实现、离线构建、定向刷写校验与短时 UART/ADC 运行冒烟。编码和构建期间实物保持断电；接线与电压预检通过后，才按冻结的 ST-LINK 序列号写入 Node B。

硬件上电前连续性与接线照片见[同日硬件证据索引](../../../hardware/H3/2026-10-08/README.md)。

## 实现边界

- 保留 H2 的 500 kbit/s、正式 `0x100/0x180` 帧、100 ms CAN 周期及错误统计；
- Node B 启动后执行一次 `HAL_ADCEx_Calibration_Start()`；
- 对 `PA0 / ADC1_IN0` 每 100 ms 执行一次轮询转换；
- 每 1 s 输出一次 `H3,B,NTC` 汇总：raw、窗口最小/最大值、累计成功/失败数、最后 HAL 结果；
- `adc_mv_nom` 使用明确标注的配置值 `VDDA=3300 mV`，不是实测 VDDA；
- `r_ntc_ohm` 使用 H0 固定电阻筛查值 `9911 Ω`；
- 不输出绝对温度，避免把未经多温点验证的 B3950 标称冒充实测精度。

## 构建结果

| 项目 | 结果 |
|---|---|
| 工程 | `firmware/node_b` |
| 工具链 | STM32CubeIDE 2.2.0；GNU Tools for STM32 14.3.1 |
| 构建方式 | IDE 生成 Makefile；单线程 `make all` |
| 退出码 | `0` |
| 编译警告/错误 | `0 / 0` |
| `text / data / bss / dec` | `33680 / 100 / 13844 / 47624` bytes |
| `node_b.elf` 文件大小 | `1095108` bytes |
| `node_b.elf` SHA-256 | `FB4B5445CA0103E64B859543D17793DBEF9568192A82ABBF029F1E4DC833605E` |
| 编译器静态帧 | `H3_AdcSample=24`、`H3_LogBoot=232`、`H3_LogNtcStatus=392`、任务入口局部帧=`32` bytes |

任务配置栈为 2048 bytes；静态帧报告支持“未见直接超栈迹象”，但不包含所有库函数最深调用链，不能替代后续实板 stack high-water mark。

规范化构建摘录见 [`H3_NODE-B_NTC_adc-build_20261008.txt`](H3_NODE-B_NTC_adc-build_20261008.txt)。

## 定向刷写与目标身份

| 项目 | 结果 |
|---|---|
| Windows 枚举 | 唯一连接的 ST-LINK 复合设备序列号 `066DFF515149856767254421`，VCP 为本次临时分配的 `COM8` |
| CubeProgrammer 只读目标 | 同一 SN；`Device ID=0x410`、`128 KBytes`、`Cortex-M3`；写前 ST-LINK 电压 `3.16 V` |
| 写入绑定 | 上表 SHA-256 对应的 `firmware/node_b/Debug/node_b.elf`，通过 SN 定向至 Node B |
| 写入与校验 | `File download complete`；`Download verified successfully`；写入时 ST-LINK 电压 `3.15 V` |
| Node A | 未连接、未枚举、未访问 |

完整的规范化控制台证据见 [`H3_NODE-B_NTC_adc-flash-verify_20261008.txt`](H3_NODE-B_NTC_adc-flash-verify_20261008.txt)。ST-LINK 电压与此前 DMM 的 `T-G=3.289 V` 属于不同仪表和时刻，均原样保留，不互相替代。

## ADC 运行冒烟

刷写后从 `COM8 / 115200 8N1` 只读采集九条连续 H3 汇总，设备 tick 跨度约 `8.208 s`：

| 指标 | 结果 | 判定边界 |
|---|---:|---|
| `cal / ready / last` | `0 / 1 / 0` | `0` 为 `HAL_OK`；校准和最近一次转换成功，ADC 已就绪 |
| `samples / fail` | `450→530 / 0` | 约 100 ms 周期持续推进，无转换失败 |
| 当前 `raw` | `1921～1939`，均值 `1927.4` | 与上电前由 DMM 电压预测的 raw≈`1932` 一致 |
| 当前 `adc_mv_nom` | 均值 `1552.8 mV` | 与 DMM `S-G=1.552 V` 一致；仍明确是按配置 3300 mV 换算 |
| 当前 `r_ntc_ohm` | `11020～11216 Ω`，均值 `11145.4 Ω` | 与同次电压比/无源阻值量级一致，不代表温度标定 |
| 各 1 s 窗口合并极值 | `raw_min=1871`、`raw_max=1951` | H3-03 继续量化偶发偏离，H3-02 不隐去也不提前判为故障 |

原始片段与统计边界见 [`H3_NODE-B_NTC_adc-runtime-smoke_20261008_run01.txt`](H3_NODE-B_NTC_adc-runtime-smoke_20261008_run01.txt)。同一片段中的 H2 CAN 统计显示无 ACK/无对端接收，这是当前单板 H3 捕获上下文，不作为 ADC 故障；它也不能替代后续双节点 CAN 回归。

## H3-03 室温基线（20:16:35～20:18:34）

执行人报告传感器已静置，并补充现场所在地气温约 `17°C`、室内可能浮动。该数值没有校准温度计背书，只作为环境背景，不作为温度标定点或 PASS 阈值。采集开始时清空串口旧缓存，此后只保存新产生的 H3 行并附主机时间戳。

| 指标 | 结果 |
|---|---:|
| H3 行数 / 主机跨度 / tick 跨度 | `117 / 118.963 s / 119.016 s` |
| 相邻报告 tick | 固定 `1026 ms` |
| `samples` / `fail` | `3330→4490`（+1160）/ `0→0` |
| 当前 raw | `1849～1897`；均值 `1865.11`；中位数 `1864`；标准差 `8.55` |
| raw P05 / P95 | `1852 / 1878` |
| 前 30 s / 后 30 s raw 均值 | `1866.97 / 1864.20`，差 `-2.77` |
| raw 线性斜率 | `-1.987 count/min` |
| `adc_mv_nom` | `1490～1528 mV`；均值 `1502.50 mV` |
| `r_ntc_ohm` | `11483～12038 Ω`；均值 `11849.41 Ω` |
| 1 s 窗口合并极值 | `raw_min=1799`、`raw_max=1917` |
| 状态完整性 | 117 条均为 `cal=0 / ready=1 / last=0 / calc_valid=1`；无 tick 复位、无采样失败 |

室温基线子项判定 **PASS**：前后 30 s 仅相差 `2.77` count，采样节拍完整且无失败。窗口短促极值仍原样保留，后续加温必须形成显著且持续的响应，不能用单点尖峰冒充趋势。为避免事后调标准，已在加温前冻结：10 条报告 rolling median raw 至少达到 `1921`；恢复后完整 30 条均值回到 `1839～1891`；全程不得出现采样失败、tick 复位或 USB/VCP 断连。

原始逐行日志见 [`H3_NODE-B_NTC_room-baseline_20261008_run01.txt`](H3_NODE-B_NTC_room-baseline_20261008_run01.txt)，计算摘要与判据见 [`H3_NODE-B_NTC_room-baseline_20261008_run01_summary.txt`](H3_NODE-B_NTC_room-baseline_20261008_run01_summary.txt)。完整 H3-03 仍为 **IN PROGRESS**，安全加温与自然恢复尚未执行。

## H3-03 手指加温—自然恢复 Run01（20:34:36～20:41:36）

按安全边界只使用手指接触封装后的 NTC 珠体，不使用任何通电加热器、热风、烙铁或 12 V。共保存 410 条 H3 报告，主机/设备跨度分别为 `419.872/419.903 s`，相邻 tick 为 `1026～1027 ms`；`samples=13860→17950`、`fail=0→0`，全部状态字段正常且无复位/串口断连。

| 指标 | 结果 | 判定 |
|---|---:|---|
| 预热 raw 均值 | `1867.12` | 与独立室温基线 `1865.11` 一致 |
| 首次 10 条 rolling median 越过阈值 | `1956`（阈值 `1921`） | PASS |
| 最大 heat-phase rolling median | `2471` | 持续响应显著，不是单点尖峰 |
| 总峰值 | raw=`2491`、`2007 mV nominal`、`6381 Ω` | 相对基线 raw/电压约 `+33.56%/+33.58%`，阻值约 `-46.15%`；方向正确 |
| 首次恢复 30 条均值进入区间 | `1889.87`；相对 release command marker `160.911 s` | PASS；该时间不是校准热时间常数 |
| 最终 30 条均值 | raw=`1867.83`、`1504.60 mV nominal`、`11817.53 Ω` | raw 仅比原基线均值高 `2.72`，位于冻结恢复区间 |

控制标记间隔为 `114.992 s`，但标记记录的是工具看到控制文件的时间，不能独立证明手指实际接触时刻；由于消息/工具延迟，本轮不能写成“精确 30 s 加温”。峰值出现在 release command marker 后，也可能同时包含珠体热惯性与实际松手消息延迟，二者未被独立分离。

Run01 的升温方向、持续阈值、自然恢复和采集完整性均 **PASS**。当日结束时仍因“可重复”要求保持 H3-03 为 **IN PROGRESS**；随后于 2026-10-09 完成自行计时 30 s 的 Run02 并关闭 H3-03，见 [`2026-10-09 Run02 证据`](../2026-10-09/README.md)。原始记录见 [`H3_NODE-B_NTC_warm-recovery_20261008_run01.txt`](H3_NODE-B_NTC_warm-recovery_20261008_run01.txt)，受控摘要见 [`H3_NODE-B_NTC_warm-recovery_20261008_run01_summary.txt`](H3_NODE-B_NTC_warm-recovery_20261008_run01_summary.txt)。

## 证据文件完整性

| 文件 | SHA-256 |
|---|---|
| `H3_NODE-B_NTC_adc-build_20261008.txt` | `4D995B5D4BBEF25BA462F14BCF1F7F4525D7D9756978F26B0594B959E73ED009` |
| `H3_NODE-B_NTC_adc-flash-verify_20261008.txt` | `3EEA64A7CF7754E2A5E2FB72A56FA716A542A0317746D53675FAA0654102C4BF` |
| `H3_NODE-B_NTC_adc-runtime-smoke_20261008_run01.txt` | `EE150F2D3456CC9D795AED58B6A73922D7E925268C06C451EB47F54AEFAE97DB` |
| `H3_NODE-B_NTC_room-baseline_20261008_run01.txt` | `3CE95530D1EE519DB65B140BDC435597567840D916F1ACCB7DBD74D8EF08A66C` |
| `H3_NODE-B_NTC_room-baseline_20261008_run01_summary.txt` | `F7445316D8032EF55B06A58539DAA557F9AA604D179409AA55841412B2716ACB` |
| `H3_NODE-B_NTC_warm-recovery_20261008_run01.txt` | `E5AB2D71DF355720064DAA3D0C7B786670D5D6784397F3EBFE9407D6446FED0D` |
| `H3_NODE-B_NTC_warm-recovery_20261008_run01_summary.txt` | `4BD47F9BAF59D3AC1E8598286F40807CB0E7FACF0615F69669DB58E24BC033A7` |

以上哈希绑定本次归档字节；若编辑对应文本，必须同步重新计算，不能沿用本表。

## 工具异常边界

首次在 Codex 执行隔离层内启动 IDE 编译器时，Windows 返回 DLL relocation Error 623，进程在源码编译前终止。同一编译器在隔离层外可正常报告版本并完成本页记录的构建，因此该事件归类为主机执行隔离兼容问题，不记为源码编译失败。

同样，CubeProgrammer 在执行隔离层内列举时返回 `No ST-Link detected!`，而 Windows PnP 已正常枚举 Node B；改在隔离层外只读列举后，CubeProgrammer 正确返回冻结序列号，随后才执行定向写入。该差异记录为 USB 调试接口的隔离层可见性问题，不写成开发板掉线。

## 当前结论

H3-02 于 **2026-10-08 PASS**：离线构建无错误/警告，目标身份闭合，指定 ELF 已定向写入并校验，ADC 校准/就绪可观察，100 ms 采样推进且短时捕获 `fail=0`，ADC 电压量级与独立 DMM 预检一致。

该结论不证明绝对温度精度、热趋势可重复性、偶发窗口极值成因、开短路诊断、双节点 CAN 回归或长时间稳定性。下一步进入 H3-03 室温静置、手指安全加温和自然恢复；若源码重新构建，必须重新计算哈希并重新绑定刷写证据。
