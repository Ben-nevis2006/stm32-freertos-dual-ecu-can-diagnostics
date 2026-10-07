# Gate H2 最小双向 CAN 测试协议与验收结果

## 1. 文档状态与时间边界

| 项目 | 内容 |
|---|---|
| 版本 | v1.0（Gate H2 关闭版） |
| 首版日期 | 2026-10-05 |
| 关闭日期 | 2026-10-07 |
| 状态 | **RUNTIME PASS / GATE H2 PASS** |
| 统一推送说明 | 本文件随 Gate H2 关闭包一次性推送；内容按真实发生日期保留 2026-10-05 的构建/首次失败和 2026-10-07 的定位、修复与验收，不代表全部工作在同一天完成 |
| 供电边界 | 仅两块 NUCLEO 的 Mini-USB 与各自 `3.3 V` CAN Pal；全部 `12 V`、Fan、Heater、INA260、NTC 和其他外设保持断开 |

本协议验证短距离双节点 CAN 物理链路、最小双向数据路径以及受控断线可观察性。它不等同于完整应用、完整故障监督或功率链验收。

## 2. 固定身份与最终接线

| 逻辑节点 | 板上标记 | CAN Pal 逻辑线 |
|---|---|---|
| Node A | `01` | `PA12/CN10-12 → TX`；`PA11/CN10-14 ← RX`；`3V3 → Vcc`；`GND → GND`；`SLNT → GND` |
| Node B | `02` | `PA12/CN10-12 → TX`；`PA11/CN10-14 ← RX`；`3V3 → Vcc`；`GND → GND`；`SLNT → GND` |

两块 CAN Pal 以 `CANH↔CANH`、`CANL↔CANL`、`GND↔GND` 相连，两端 termination 均置 `ON`。最终恢复验收前，在全断电条件下从两端分别测得 `CANH-CANL=60.3 Ω`、`60.4 Ω`。

> 重要修订：首版中的 `SLNT 悬空` 已被实测否定。TJA1051T/3 模块在本项目中必须把 `SLNT` 明确下拉到 GND；悬空或接触不稳会导致静默/不确定工作状态。

烧录目标以 ST-LINK 身份映射为准；COM 号和电脑物理 USB 口仅是当次枚举结果，不作为永久身份。

## 3. 位时序与运行方式

- 系统时钟：ST-LINK MCO 提供 `8 MHz` 外部时钟，MCU 使用 `HSE bypass + PLL×8`，`SYSCLK=64 MHz`。
- CAN 内核时钟：APB1 `32 MHz`。
- bxCAN：Prescaler=`4`、SJW=`1 TQ`、BS1=`13 TQ`、BS2=`2 TQ`。
- 每位 `16 TQ`，名义速率 `32 MHz / (4 × 16) = 500 kbit/s`，采样点 `87.5%`。
- Normal 模式、11 位标准 ID、数据帧、自动重发和自动 Bus-Off 恢复开启。
- 两节点 CAN 启动后延时 `10 s`，再按 `100 ms` 周期发送。
- FIFO0 中断接收；USART2/ST-LINK VCP 使用 `115200 8N1`，每秒输出状态。

将时钟从 HSI 派生切换为板载 ST-LINK 的 8 MHz MCO，是为了消除两块实板内部 RC 振荡器偏差对 500 kbit/s CAN 容差的影响；名义参数计算不能替代实板验收。

## 4. 两阶段帧定义

### 4.1 排障阶段固定帧（历史基线）

| 方向 | ID | DLC | 周期 | 用途 |
|---|---:|---:|---:|---|
| Node A → Node B | `0x321` | 8 | 100 ms | 节点标记、8 位计数器、反码和固定签名 |
| Node B → Node A | `0x322` | 8 | 100 ms | 节点标记、8 位计数器、反码和固定签名 |

该配置用于隔离纯收发链路，2026-10-07 在修复 SLNT 接触后连续完成 3 轮 `80 s` 双向通过。当前上板固件已经切换为正式基线，不再使用 `0x321/0x322`。

### 4.2 正式基线帧（当前上板）

| 方向 | 标准 ID | DLC | 周期 | 测试向量 |
|---|---:|---:|---:|---|
| Node B → Node A | `0x100` Cooling_Command | 2 | 100 ms | byte0=`0x00`（demand=0）；byte1 低 4 位=`AliveCounter`，高 4 位保留为 0 |
| Node A → Node B | `0x180` Actuator_Status | 5 | 100 ms | byte0=`0x00`；byte1=`0x00`；byte2 低 4 位=`AliveCounter`、高 4 位 0；byte3/4=`0xFF/0xFF`（RPM 无效） |

双方均检查标准 ID、DLC、语义/保留位和 4 位 AliveCounter 连续性；计数器按 `0…15` 回绕。第一帧只建立序列基线，不计跳号。为满足 STM32 HAL 的缓冲区访问要求，发送缓冲区仍分配 8 字节并先清零，线上 DLC 保持 2/5。

## 5. 串口证据字段

启动行示例：

```text
H2,A,BOOT,profile=formal_baseline,bitrate=500000,tx_id=0x180,tx_dlc=5,rx_id=0x100,rx_dlc=2,period_ms=100,start_delay_ms=10000
H2,B,BOOT,profile=formal_baseline,bitrate=500000,tx_id=0x100,tx_dlc=2,rx_id=0x180,rx_dlc=5,period_ms=100,start_delay_ms=10000
```

- `tx_try/tx_q/tx_fail`：发送尝试、成功交给邮箱、邮箱提交失败次数。
- `rx_irq/rx/rx_bad`：FIFO0 中断、通过全部校验的帧、不合格帧。
- `seq_err/q_drop`：AliveCounter 跳号、HAL 取帧或 RTOS 队列投递失败。
- `seen/last`：是否已建立对端序列基线及最近计数器。
- `mb_free`：空闲发送邮箱数。
- `hal_err/esr`：HAL 累积错误与 bxCAN ESR 原始值；ESR 同时用于提取 TEC、REC、EWGF、EPVF、BOFF。

## 6. 通过准则

稳定窗口从双方出现 `BOOT` 且 `10 s` 发送延时结束后开始，至少 `60 s`：

- 两侧 `tx_q`、`rx` 持续增长，约 `10 帧/s`，且 `seen=1`；
- 末态 `tx_fail=0`、`rx_bad=0`、`seq_err=0`、`q_drop=0`；
- 不出现持续 Error Warning、Error Passive 或 Bus-Off，末态 ESR 回到 0；
- Windows 不反复断连，两块板和 CAN Pal 无异味、异常发热或火花；
- 受控断开 H/L 时必须在串口错误计数/状态中清晰可观察；恢复后必须重新通过完整稳定窗口。

## 7. 最终验收结果

| 阶段 | Node A | Node B | 判定 |
|---|---|---|---|
| 固定帧重复性（3×80 s） | 每轮约 `tx/rx=687/688`，四项错误为 0 | 每轮约 `690/689`，四项错误为 0 | PASS |
| 正式基线（80 s） | `687/688`；max TEC/REC=`16/0`；末态 ESR=0 | `690/689`；max TEC/REC=`4/0`；末态 ESR=0 | PASS |
| H/L 受控断开（30 s） | `rx=0`、`tx_fail=194`、进入 Warning/Passive | `rx=0`、`tx_fail=187`、出现 Warning/Passive/Bus-Off | 故障被明确观测 |
| 接线复核后恢复预检（30 s） | `197/198`；四项错误为 0 | `190/189`；四项错误为 0 | PASS |
| 最终恢复（80 s） | `687/688`；max TEC/REC=`6/0`；末态 ESR=0 | `690/689`；max TEC/REC=`6/0`；末态 ESR=0 | **PASS** |

原始日志、逐文件 SHA-256 和失败/恢复过程见 [`2026-10-07 软件证据索引`](../evidence/software/H2/2026-10-07/README.md)。

## 8. 结论边界

Gate H2 在 2026-10-07 判定为 **PASS**：低压双节点 CAN 物理链路、正式基线帧、计数器、错误状态可观察性及断线后恢复均有原始日志支持。

尚未由本 Gate 证明：`300 ms` 应用超时、两次异常确认/三次有效恢复等完整故障监督策略，12 V 功率链、Fan/Heater/INA260/NTC、负载条件、长时间耐久、EMC 或生产级可靠性。这些必须在后续 Gate 单独验收。
