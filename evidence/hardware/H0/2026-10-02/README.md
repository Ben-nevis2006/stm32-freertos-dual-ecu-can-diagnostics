# Gate H0 D-02 CAN 收发器顶标补拍与关闭证据索引（2026-10-02）

## 1. 本批证据目的

本目录保存 `CANPAL-01`、`CANPAL-02` 正面中央 SOIC-8 收发器芯片的两轮补拍原图。首轮照片保留证据不足的事实；第二轮原尺寸照片用于关闭 D-02 中适用于 Gate H2 的 CAN 收发器实体顶标子项。照片不证明 CAN 电气性能或通信功能。

## 2. 证据清单与 SHA-256

| 归档文件 | 原始文件名 | 字节数 | 用途/判定 | SHA-256 |
|---|---|---:|---|---|
| `H0_CANPAL-01_chip-top-unreadable_20261002_01.jpg` | `6f46e3a45f3956e8e3a48676e345fcba.jpg` | 245453 | 首轮尝试；被后续清晰原图替代 | `A413BA71607A3BF0AA28A47DBD5EAC792AA2A51FD96DE98735E9437ABDAD8611` |
| `H0_CANPAL-02_chip-top-unreadable_20261002_01.jpg` | `1a34e7e17e16f42fe22dcb477a03e802.jpg` | 118220 | 首轮尝试；被后续清晰原图替代 | `82650593F5E449396DA7E95DA85A51C4323F3B65019699D357C65F093F3015A3` |
| `H0_CANPAL-01_chip-top-readable_20261002_01.jpg` | `b8f2b5ca053ed64e9b870709f82f16a4.jpg` | 3387322 | 第二轮；顶标关闭证据 | `EEBD4D3A88F8948BB42531ABD2E3BAB3B4B6CA0014A633850EDFE12688C3C621` |
| `H0_CANPAL-02_chip-top-readable_20261002_01.jpg` | `61a2cd518b5bd14b85e83a7c94220d7b.jpg` | 2362325 | 第二轮；顶标关闭证据 | `C64529E6D32C65E3CDB5F5B0F5269C95FFBDCB41EA55FBD110DD8433454AF101` |

所有原图均按字节原样归档，仅规范化文件名；未裁剪、未修图、未覆盖原文件。分析时使用的旋转/裁剪视图没有作为原始证据归档，生成式增强结果未参与判定。

## 3. 可复核事实

- 第二轮两张照片均显示目标模块正面、端子手写 `01/02` 对应关系及绿色端子下方、终端开关左侧的中央 SOIC-8 芯片；
- 两颗中央 SOIC-8 均可辨 NXP 标志和型号主标 `A1051/3`；批次/追溯行不作为本次型号判定条件；
- 主标与两块 PCB 背面已记录的 `TJA1051T/3` 模块声明一致，形成实装芯片与板级规格的交叉证据；
- 正面 `Vcc/GND/RX/TX/SLNT/CANH/CANL`、终端开关和背面 `3-5V Vin/logic w/5V boost`、`Optional 120Ω termination` 已分别在前序证据中留痕；
- 静态照片不证明供电、收发、总线波形、误码率或双向通信正常。

## 4. 判定与边界

第二轮照片判定为 **PASS FOR CAN TRANSCEIVER IDENTITY**：

- D-02 的 `CANPAL-01/02` 实体芯片顶标子项：**CLOSED 2026-10-02**；
- D-02 的 CAN/H2 接线前身份子范围：**CLOSED 2026-10-02**；
- D-02 的 INA260、Fan、Heater、Heatsink 和辅材子项：继续 **OPEN**，分别在对应后续 Gate 前关闭；
- 允许进入 H2 的全断电、短线、两端终端和约 `60 Ω` 无源检查；本结论不授权 12 V 动作，也不等同于 H2 PASS。

## 5. 参考资料

- Adafruit CAN Pal 产品页：<https://www.adafruit.com/product/5708>
- Adafruit CAN Pal Pinouts：<https://learn.adafruit.com/adafruit-can-pal/pinouts>
- NXP TJA1051 产品页：<https://www.nxp.com/products/interfaces/can-transceivers/can-with-flexible-data-rate/high-speed-can-transceiver:TJA1051>
- NXP TJA1051 数据手册：<https://www.nxp.com/docs/en/data-sheet/TJA1051.pdf>
