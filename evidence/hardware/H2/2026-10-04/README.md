# Gate H2 累计证据索引（2026-10-04）

## 当前状态

这是 **2026-10-04 的阶段快照**：当日 Gate H2 为 IN PROGRESS。Node B 单板稳定性隔离和双板同时 USB 上电 `2 min` 均已通过；CANPAL-02 的 `Vcc-GND` 实测为正常的 `3.300 V`。先前手持探针期间的 `LD1/COM` 红闪与 Windows USB 断连/重连同步，确认发生过 ST-LINK 重新枚举；静置和当时双端口配置下均未复现。设备身份已固定，当日转入最小 CAN 测试固件准备阶段。

本记录在 Gate 完成后的统一推送中保留原日期和阶段判定，不回填成“当日已通过”。H2 后续于 2026-10-07 完成多故障修复、正式帧、断线注入和恢复验收，最终 PASS 见 [`2026-10-07 软件证据`](../../../software/H2/2026-10-07/README.md)。

引脚判读依据 ST 官方 [UM1724（STM32 Nucleo-64 boards，MB1136）](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf) 的 NUCLEO-F103RB 布局图与 ST Morpho connector 表。

## 证据清单

| 归档文件 | 原始文件名 | 字节数 | SHA-256 | 可支持的事实 | 不能支持的结论 |
|---|---|---:|---|---|---|
| `H2_NODE-B_CANPAL-02-header-wiring_20261004_01.jpg` | `7dcc1fcfe951baae2efe81fd664e8b23.jpg` | 126160 | `126E51C620DAFBE07EA9D7221BFE898481C51F12DE79C04419DA2D947B90D74C` | CANPAL-02 排针端黑/白/黄/棕分别占用 `Vcc/GND/RX/TX`，`SLNT/CANH/CANL` 排针留空 | 不证明跳线另一端正确、焊点导通或 CAN 通信 |
| `H2_NODE-B_CN10-signal-pin-closeup_20261004_01.jpg` | `511b3243f634c0cb73b569311186fe5f.jpg` | 237532 | `C08AB6F58561559E86C18A5251F1CB227A39501A989782955F4646FFB3DE86DA` | 棕线位于 CN10 外侧偶数列 `CN10-12/PA12`，黄线位于下一行 `CN10-14/PA11` | 不证明线路导通、CAN 位时序或报文收发 |
| `H2_NODE-B_power-pin-closeup_20261004_01.jpg` | `39ceb673946ca03e1f5e67ab24199fda.jpg` | 327681 | `5ABDAABE0E6EC03A1A710EC65D5F062240D01379E600F215D1FEE5C66312D58D` | 黑线落在 CN7 偶数列 `+3V3`，白线落在同列 `GND`；未插 USB | 不证明 Vcc-GND 无短路、实际供电电压或上电稳定性 |
| `H2_NODE-B_CANPAL-02_Vcc-GND-resistance-overrange_20261004_01.jpg` | `9a65d98555a05b1b691b531157919ed4.jpg` | 218959 | `F1B011AEFF5022FFAC3B338FFC4C3AF44F40A7D6D43A036729DA6D552798C0FA` | 完整 Node B 接线、全断电状态下，`200 Ω` 档跨接 CANPAL-02 的 `Vcc-GND` 显示 `1`（超量程），未发现低阻短路 | 不证明实际供电电压、上电稳定性、TX/RX 连续性或 CAN 通信 |
| `H2_NODE-B_controlled-USB-power_photo_20261004_01.jpg` | `b84bcfccfe2fca6ae97a733e339bbb67.jpg` | 210481 | `7C17B2CF35FF5D50FE5D283575F4A2583A75B74DEB6EDD09EFC7672B29E2E04C` | 仅 NUCLEO-02 接入 Mini-USB，Node A 保持未上电；NUCLEO-02 电源/状态灯可见，未见 `12 V` 功率链接入 | 静态照片不能证明 30 s 内无异味、无火花、无异常发热、Windows 无反复断连，也不证明 CANPAL-02 实际电压或 CAN 通信 |
| `H2_NODE-B_CANPAL-02_Vcc-3V300_probe-incident_20261004_01.jpg` | `476935aff3c336e6aab8431babfb5446.jpg` | 245041 | `1C870CA1987EE1488D8AEDF71C1DD1E381808266EFBA039C40627E2D43226B9F` | 直流 `20 V` 档跨接 CANPAL-02 的 `Vcc-GND`，照片显示 `3.300 V`，位于 `3.2～3.4 V` 接受区间 | 单张照片不能证明长时间稳定；不能判定用户报告的 `LD1/COM` 红闪究竟由探针瞬间碰邻点、线束/USB 被牵动还是其他供电/通信因素引起 |
| `H2_WINDOWS_STLINK-enumeration_20261004.txt` | Windows PnP 只读查询记录 | 1542 | `6F08E08D6BCC216026A09B96599AFC2BEAF848791CDC4E8FBBC977BD0398708E` | 两个 ST-LINK 复合设备、Debug 和 VCP 均为 `OK`；以序列号和受控接入顺序固定 Node A/Node B 身份 | 不证明固件身份、CAN 通信或以后仍使用相同 COM 号/物理 USB 端口 |

## 现场文字观察

- `2026-10-04 / H2-07A`：用户确认“30 秒观察与单片上电现象相同，无异常”。该文字记录与受控上电照片组合，支持 Node B 本次 30 s 上电未出现相对 H1 基线的新异常；不证明 CANPAL-02 的实际 Vcc 或 CAN 通信。
- `2026-10-04 / H2-07B`：用户确认 CANPAL-02 电压正常，并报告由于单人同时持表笔和手机，测量过程较长且表笔容易滑动；期间 `LD1/COM` 有时在尚未完成测量前由黄/橙色转为红色闪烁，移走表笔后仍持续，只有断电再上电才恢复。依据 ST 官方 UM1724 §7.6，`LD1/COM` 反映 ST-LINK 通信状态而非 CAN 总线错误；本记录将其列为待隔离的供电/USB/操作扰动，不推断已经发生短路或器件损坏。
- `2026-10-04 / H2-07C`：移走表笔且完全不触碰 Node B 和线束后，用户确认连续 `2 min` 稳定；用户同时补充 H2-07B 的红闪与 Windows 断连/重连同步。该组合证据确认先前发生过 ST-LINK USB 重新枚举，并支持其与测量/机械操作相关；不能据此唯一判定具体瞬态路径，也不证明 CAN 报文收发。
- `2026-10-04 / H2-08A`：两板同时上电并静置后，用户确认“两块板 2 分钟稳定”。为腾出第二个 USB 口，用户拔下了 2.4 GHz 无线鼠标，并说明当前使用端口可能不同；因此本次结论限定为“当前双端口配置稳定”，不排除原端口/插头/线缆因素。

原图按字节原样归档，仅规范化文件名；未裁剪、未修图、未覆盖原文件。
