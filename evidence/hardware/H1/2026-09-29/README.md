# Gate H1 现场证据索引（2026-09-29）

## 1. 本批证据目的

本目录保存 `NUCLEO-01` 在 STM32CubeProgrammer 中完成 ST-LINK 探针发现、但尚未点击 `Connect` 时的只读界面截图。此前的上电前、Windows 枚举和 30 s 稳定性证据见 [2026-09-27 证据索引](../2026-09-27/README.md)。

## 2. 证据清单与 SHA-256

| 归档文件 | 原始文件名 | 字节数 | SHA-256 |
|---|---|---:|---|
| `H1_NUCLEO-01_cubeprogrammer-probe-panel_20260929_01.png` | `codex-clipboard-642ba55f-4cff-4b52-86b0-41e264b8dd23.png` | 56617 | `D4DA31EA55967A52D924F725409E74BD6E853B29BA9DD46024BCE168BD7ED2B1` |
| `H1_NUCLEO-01_cubeprogrammer-target-connected_20260929_01.png` | `codex-clipboard-5a0f4e84-e986-4779-9c62-60d752e3e66a.png` | 400446 | `95C209A83D78FB0A33637F88ADFD9B56337E7B0D6D45A1ED778A9E7DD8439D28` |
| `H1_NUCLEO-01_cubeprogrammer-connection-log_20260929_01.png` | `codex-clipboard-65f53f73-e0bd-41ae-9276-948b0ab3b0ef.png` | 156846 | `24EE3EADB821EEE21FC1B055F75EFFC607D1452A0801CCA693618DC5F73AE137` |
| `H1_tool_cubeprogrammer-about_20260929_01.png` | `codex-clipboard-0948fa2e-1d84-4648-8d95-d89f0023275e.png` | 19165 | `BB2EBE73866402E2CE0821D1BABA06920E35A059AF5CDFCBE9A8778327BA30B7` |

原图按字节原样归档，仅规范化文件名；未裁剪、未修图、未覆盖原文件。

## 3. 可核验事实

| 字段 | 截图值 | 判定 |
|---|---|---|
| ST-LINK Serial Number | `066BFF575…`（界面截断显示） | 探针已被 CubeProgrammer 发现；完整序列号仍待连接日志取证 |
| Port | `SWD` | 保持默认 |
| Frequency | `4000 kHz` | 保持默认 |
| Mode | `Normal` | 保持默认 |
| Access port | `0` | 保持默认 |
| Reset mode | `Software reset` | 保持默认 |
| Speed | `Reliable` | 保持默认 |
| Shared | `Disabled` | 保持默认 |
| Target voltage | `3.24 V` | 与 3.3 V 目标供电相符，可放行只读连接 |
| ST-LINK firmware | `V2J46M32` | 已记录；本 Gate 不执行升级 |

截图可见 `Firmware upgrade` 按钮，但没有证据表明已点击或执行升级；H1 明确禁止升级。

## 4. 判定与边界

`NUCLEO-01` 的 CubeProgrammer 探针发现步骤判定为 **PASS**。该截图证明主机能够发现 ST-LINK、读取当前固件版本和目标电压，但尚不能证明：

- 已成功连接目标 MCU；
- 目标 MCU / Device ID 与 NUCLEO-F103RB 一致；
- Flash 容量正确；
- 本项目固件或任何冻结功能正在运行。

## 5. 目标只读连接结果

| 字段 | 截图值 | 判定 |
|---|---|---|
| Connection state | `Connected` | PASS |
| Device | `STM32F101/F102/F103 Medium-density` | 与 NUCLEO-F103RB 所属产品线一致 |
| Type | `MCU` | PASS |
| Device ID | `0x410` | 与 Medium-density STM32F101/F102/F103 产品线一致 |
| Revision ID | `Rev X` | 已记录 |
| Flash size | `128 KB` | 与 STM32F103RB 基线一致 |
| CPU | `Cortex-M3` | 与 STM32F103 基线一致 |
| Target voltage | `3.24 V` | 连接后仍正常 |
| ST-LINK firmware | `V2J46M32` | 连接后仍一致；未升级 |
| Flash read | `0x08000000` 起始、`1024 Bytes`、`Data read successfully` | 只读访问 PASS |

CubeProgrammer 日志使用 `UPLOADING` 描述本次从 MCU 到主机的读取过程，这不等于向 MCU 下载或烧写。截图没有显示 Erase、Download 或 Option Bytes 写操作。读出的向量表和 Flash 内容只能证明目标内存在可读数据，不能证明它是本项目固件。

Device ID `0x410` 不能单独区分 STM32F101/F102/F103 的所有具体型号；本记录依据 Device ID、128 KB Flash、Cortex-M3、板卡正面丝印和既有 H0 身份证据共同判断其与 NUCLEO-F103RB 基线一致。截图中的 Board 字段未给出具体板型，不据此推导故障或真伪结论。

## 6. 完整连接日志与工具版本

| 字段 | 证据值 |
|---|---|
| STM32CubeProgrammer | `v2.19.0 / Windows-64Bits` |
| ST-LINK SN | `066BFF575151676667043206` |
| ST-LINK FW | `V2J46M32` |
| Voltage | `3.24 V` |
| SWD frequency | `4000 KHz` |
| Connect / Reset mode | `Normal / Software reset` |
| Device / Revision ID | `0x410 / Rev X` |
| Disconnect | `20:58:51 : Disconnected from device.` |

日志中的 `UPLOADING OPTION BYTES DATA` 表示 CubeProgrammer 自动读取 Option Bytes 数据，不是 Option Bytes 写入。日志未显示 Erase、Download 或写 Option Bytes。`Debug in Low Power mode is not supported for this device.` 只说明该低功耗调试选项不受目标支持；低功耗调试不在 H1 范围内，常规 SWD 连接和读取已经成功。

## 7. NUCLEO-01 结论与下一证据

`NUCLEO-01` 单板 H1 结论：**PASS**。范围仅限 USB 枚举、ST-LINK/VCP、30 s 稳定性、无异常状态、目标识别和只读访问。

下一证据从 `NUCLEO-02` 的上电前正面照开始。必须先关闭 CubeProgrammer、拔除并移走 `NUCLEO-01`；不得复制 `NUCLEO-01` 的 COM 号、序列号或截图作为 `NUCLEO-02` 证据。
