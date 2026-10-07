# Gate H2 累计证据索引（2026-10-03）

## 当前状态

这是 **2026-10-03 的阶段快照**：当日 Gate H2 为 IN PROGRESS。本目录记录 CAN Pal 排针焊接、Node A 上电前逻辑接线、无源排短路与受控上电；CANPAL-01 实测供电 `3.288 V`，CANPAL-02 的排针视觉检查及 Vcc-GND 无源排短路已通过，当日尚未证明 CAN 通信。记录在统一推送中保留原日期和阶段边界；H2 后续于 2026-10-07 PASS，见 [`2026-10-07 软件证据`](../../../software/H2/2026-10-07/README.md)。

## 证据清单

| 归档文件 | 原始文件名 | 字节数 | SHA-256 | 可支持的事实 | 不能支持的结论 |
|---|---|---:|---|---|---|
| `H2_CANPAL-01_header-solder-top_20261003_01.jpg` | `b230d0af750e1486201aca05d118f3c1.jpg` | 117181 | `058B1C2C93DC3C332A7D586F62A25D90F914994D601DDA95A04511C6413FB5CB` | CANPAL-01 七个排针焊点彼此可分辨，未见明确跨焊；termination 仍在 `ON` 一侧 | 不能排除冷焊、隐藏连锡或内部未润湿；不证明导通和通信功能 |
| `H2_CANPAL-01_header-solder-side-and-jumpers_20261003_01.jpg` | `66d220ee44be0ddc046d785d18afb5e6.jpg` | 99455 | `3E39EC1DC7ED21DDB5E1CA5281E600C08B19DCD7FA22995CBE675706D9F2369C` | 排针塑料座基本贴板；CANL、CANH、SLNT 三针空置，TX/RX/GND/Vcc 四针已接跳线 | 不能证明四根跳线另一端落点正确，也不能证明焊点电气可靠 |
| `H2_NODE-A_pre-power-logic-wiring-oblique_20261003_01.jpg` | `3d19e051f6337f124305f3d62dbd180a.jpg` | 313510 | `BC2158EE76BEA9DB0B4193FE6929B5B352156E6371977F3A8DEEBE64D65759B7` | NUCLEO-01 处于未插 USB 状态并已接四根逻辑线；未见 12 V | 斜拍和跨线遮挡使 CN10 行号及 CN6 的 3V3/GND 落点无法被独立确认，因此不能放行上电 |
| `H2_NODE-A_CN10-signal-pin-closeup_20261003_01.jpg` | `24c012c4a298bfcef802bae6a598d500.jpg` | 183681 | `9F59D8033846D04611F82CE9629D1955CCA6EB6BA8050C6A70330B4CE3FB05A9` | 棕线位于 `PA12/CN10-12`，黄线位于其紧邻下方 `PA11/CN10-14`；对应 CANPAL-01 的 TX、RX | 不证明线路导通、焊点可靠、CAN 位时序或报文收发 |
| `H2_NODE-A_CN6-power-pin-closeup_20261003_01.jpg` | `2dd69f01e21742b603b64a5e945216c8.jpg` | 201971 | `6952653485EAC1B4CD1775090CA5E502145B96BB6AE513C8F109A961D2CE25F2` | 黑线落在 CN7 偶数列 `+3V3`，白线落在同列 `GND`；未插 USB；历史归档文件名中的 `CN6` 保留 | 不证明 Vcc-GND 无短路、实际供电电压、焊点可靠或上电稳定性 |
| `H2_NODE-A_CANPAL-01_Vcc-GND-resistance-overrange_20261003_01.jpg` | `ae5a2ac776ccd157df07d5ac0037a0c2.jpg` | 4336874 | `73ECC978F39DE1CB039215AF213A58B4B74387CF6EA548B01A4B9BE155022D8E` | USB 与 `12 V` 断开时，`200 Ω` 档跨接 CANPAL-01 的 `Vcc-GND` 显示 `1`（超量程），未发现低阻短路 | 不证明供电电压、焊点导通、上电稳定性、CAN 位时序或报文收发 |
| `H2_NODE-A_CANPAL-01_Vcc-3V288_20261003_01.jpg` | `6f8099f0945b90504822fe24db9fb27e.jpg` | 249578 | `A91484D36BCF474A50BF33852D7C2BC7395F41A66EA1AA7185EBAB53399D7F62` | Node A USB 上电时，CANPAL-01 的 `Vcc-GND` 实测 `3.288 V`，位于预期范围内 | 不证明 TX/RX 焊点导通、CAN 位时序、报文收发或 CANPAL-02 供电 |
| `H2_CANPAL-02_header-solder-side_20261003_01.jpg` | `f27f8816ae2096b749c4627e5529938f.jpg` | 95423 | `11F4D9D81BA3AA0A7DEEECF42D0AF730F0796A2C2856097D0CED5FCFFD89F8CF` | CANPAL-02 排针塑料座基本贴板，七根针脚排列可见 | 不证明各针电气导通、无隐藏短路或焊点内部润湿充分 |
| `H2_CANPAL-02_header-solder-top_20261003_01.jpg` | `d2d84488511d23fd7a76878aeea9bd60.jpg` | 108579 | `4B7154B9F6DACF8C7962E9B7554A8A5D91397AEC10CF5033D5A922B9E1DBB106` | 七个焊点彼此可分辨，未见明确跨焊；锡量偏大且焊点外形不均匀 | 不能排除隐藏连锡、冷焊或内部未润湿；不证明供电与通信功能 |
| `H2_CANPAL-02_Vcc-GND-resistance-overrange_20261003_01.jpg` | `0d65f64e719887c67e8c81123ef9dd97.jpg` | 181078 | `C799439AFE255E48F7B865A71AB36CCD3AF1CCA0C5A34C1C1A3C625F601E0EF1` | 全断电时，`200 Ω` 档跨接 CANPAL-02 的 `Vcc-GND` 显示 `1`（超量程），未发现低阻短路 | 不证明焊点导通、实际供电电压、上电稳定性或 CAN 通信 |

## 现场文字观察

- `2026-10-03 / H2-04A`：用户报告 Node A 接入 Mini-USB 后，现象与 H1 独立上电时一致，两块接在无源总线上的 CAN Pal 均无可见现象。该记录支持本次受控上电未出现新的可见异常；CAN Pal 本身无状态指示灯，且尚未运行 CAN 收发程序，因此“无可见现象”不证明故障，也不证明已正确供电或能够通信。

原图按字节原样归档，仅规范化文件名；未裁剪、未修图、未覆盖原文件。
