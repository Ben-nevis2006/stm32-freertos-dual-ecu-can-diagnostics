# Gate H1 现场证据索引（2026-09-27）

## 1. 本批证据目的

本目录保存 `NUCLEO-01` 进入 Gate H1 前的原始照片，用于闭环 H1-00 的板卡身份、外观、外接线隔离和 USB 插头形态检查。原图按字节原样归档，仅规范化文件名；未裁剪、未修图、未覆盖原文件。

## 2. 证据清单与 SHA-256

| 归档文件 | 原始文件名 | 字节数 | SHA-256 | 可核验事实 |
|---|---|---:|---|---|
| `H1_NUCLEO-01_front-before-usb_20260927_01.jpg` | `3a2300e35e5d8f0d156af60a13b9508d.jpg` | 159876 | `2E545C876BEE5A613FA74EA23475B96D1D1B49EAC59F204A6273FEE76F0502E8` | 可见手写 ID `01`、板卡正面、CN1 未连接、排针无外接线；线缆设备端因侧面角度不能仅凭此图可靠判型 |
| `H1_NUCLEO-01_cable-connector-confirmation_20260927_01.jpg` | `f97fbe681769906cfc25244909447374.jpg` | 202265 | `C1DC1130496A980E77EC42D4903E24887A121463CF09D724C994CBF0C47025E7` | 清楚可见 USB-A 与 USB Mini-B 两端；Mini-B 插头形态与 NUCLEO-F103RB 板载 ST-LINK 的 CN1 接口匹配 |
| `H1_NUCLEO-01_powered-leds_20260927_01.jpg` | `98c39068635455b672e3c54721902b8c.jpg` | 190663 | `B5CC0E1BD8D6CFE4B9BA7155791F2B32DAAC8C26A690CD46E08244B19E842089` | NUCLEO-01 通过 CN1 单独 USB 供电，未见其他外接线；静态照片可见板上指示灯点亮 |
| `H1_NUCLEO-01_windows-vcp-COM7_20260927_01.png` | `4334f04362b7c6d482d115df7e9f098b.png` | 13178 | `EE4D72A30EF073F551EADBBA96F214AB1DBEA9C8C3DB15FB9133E25DF25BE835` | Windows 设备管理器显示 `STMicroelectronics STLink Virtual COM Port (COM7)` |
| `H1_NUCLEO-01_windows-stlink-debug_20260927_01.png` | `c1ef4617bf2557e5ee1bd045dc7857aa.png` | 7782 | `97F42664C8155A663ED889E8F4221D20ACBCA72B4E5AEB0C57266256C9F5B87C` | Windows 设备管理器显示 `ST-Link Debug` |

## 3. H1-00 判定

| 检查项 | 判定 | 依据与边界 |
|---|---|---|
| 实体身份 | PASS | 板上丝印 `NUCLEO-F103RB / NUF103RBSAU1`，手写 ID `01` 可见 |
| 板卡外观与接口 | PASS（照片可见范围） | 未见明显破损、异物或接口机械损伤；静态正面照不替代通电功能验证 |
| 外部连接隔离 | PASS（照片可见范围） | CN1、Arduino/Morpho 排针均未连接；照片拍摄时未上电 |
| USB 插头机械规格 | PASS | 补拍照片确认设备端为 USB Mini-B，不是 DC 圆头 |
| USB 数据能力 | PASS | Windows 同时枚举 ST-Link Debug 和 STLink Virtual COM Port，证明线缆具备有效数据通路 |

结论：`NUCLEO-01` 的 H1-00 照片预检通过；随后形成的 Windows 截图证明 ST-LINK 调试接口、VCP/COM7 与 `USBCABLE-01` 数据通路已经枚举。目标 MCU 身份、Flash 容量和 CubeProgrammer 只读连接仍未验证。

## 4. H1-01 阶段性现场记录

- 执行人报告：`LD2` 约每秒闪烁一次；`LD1` 刚上电时闪烁一段时间，随后常亮；
- Windows 枚举结果：`ST-Link Debug` 与 `STMicroelectronics STLink Virtual COM Port (COM7)`；
- 工程边界：LD2 周期闪烁只说明某段既有程序可能在驱动 LED，不能证明其来源是本项目，也不能替代 FreeRTOS、CAN、Fault 或其他冻结功能的验证；
- 尚待执行人明确确认：30 s 内无异味、异常发热、火花或 Windows 反复断连；
- 尚待 CubeProgrammer 只读取证：ST-LINK 序列号、固件版本、目标电压、目标 MCU / Device ID 与 Flash 容量。

## 5. 识别更正留痕

第一张照片中 Mini-B 插头近似正对侧面，曾被误判为 DC 圆头。第二张补拍照片展示了完整金属壳体和 Mini-B 轮廓，已据此更正。工程结论以后者和实际枚举结果为准；不删除第一张照片，以保留判断更正链路。

## 6. 下一证据

H1-01 需要继续归档：

1. 至少 30 s 无反复掉线的明确现场确认；
2. STM32CubeProgrammer 的探针序列号、固件版本、目标电压、目标 MCU / Device ID 和 Flash 容量只读截图；
3. 明确记录无异味、异常发热、火花或插头松动。

在上述证据形成前，不得把 H1-01 或 `NUCLEO-01` 写成 PASS。
