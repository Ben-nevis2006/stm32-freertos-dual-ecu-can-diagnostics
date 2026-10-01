# Gate H1 现场证据索引（2026-10-01）

## 1. 本批证据目的

本目录保存 `NUCLEO-02` 的 STM32CubeProgrammer ST-LINK 探针发现截图。此前的上电前照片、Windows ST-LINK/VCP 枚举和 30 s 稳定性证据见 [2026-09-29 证据索引](../2026-09-29/README.md)。

## 2. 证据清单与 SHA-256

| 归档文件 | 原始文件名 | 字节数 | SHA-256 |
|---|---|---:|---|
| `H1_NUCLEO-02_cubeprogrammer-probe-panel_20261001_01.png` | `codex-clipboard-bbb13a8b-855c-4976-bc5b-ac08ef221ce7.png` | 61316 | `FF618DD38A9A1E6CC259E1B0729F06A5325D4DAC9EF6C9B12431422E9C77ECFF` |
| `H1_NUCLEO-02_cubeprogrammer-target-connected_20261001_01.png` | `codex-clipboard-d98fe91a-b70b-4ce8-9b99-fedc7a49addc.png` | 308229 | `BBE769ACFC38C6F24044F0E57414BAE778B85BC333916C709E98D898D19237BB` |
| `H1_NUCLEO-02_cubeprogrammer-disconnected-log_20261001_01.png` | `codex-clipboard-f9cb36fa-7618-4ec9-8292-befb69cc0289.png` | 318297 | `45F47980B73A4C7F2EDDC8CCBA26769B7D79EB0019FC59722003DB78EE1A81E8` |

原图按字节原样归档，仅规范化文件名；未裁剪、未修图、未覆盖原文件。

## 3. 可核验事实

| 字段 | 截图值 | 判定 |
|---|---|---|
| ST-LINK Serial Number | `066DFF515…`（界面截断显示） | 探针已被发现；完整序列号待连接日志取证 |
| 与 NUCLEO-01 的区分 | NUCLEO-01 前缀为 `066BFF575…` | 两个可见前缀不同，支持独立探针的初步区分 |
| Port | `SWD` | 保持默认 |
| Frequency | `4000 kHz` | 保持默认 |
| Mode | `Normal` | 保持默认 |
| Access port | `0` | 保持默认 |
| Reset mode | `Software reset` | 保持默认 |
| Speed | `Reliable` | 保持默认 |
| Shared | `Disabled` | 保持默认 |
| Target voltage | `3.24 V` | 与 3.3 V 目标供电相符，可放行只读连接 |
| ST-LINK firmware | `V2J46M32` | 已记录；本 Gate 不执行升级 |

## 4. 判定与边界

`NUCLEO-02` 的 CubeProgrammer 探针发现步骤判定为 **PASS**。截图可见 `Firmware upgrade` 按钮，但没有证据表明已点击或执行升级。当前证据证明主机能够发现第二块 ST-LINK，并读取探针固件版本和目标电压；尚不能证明目标 MCU / Device ID、Flash 容量、Flash 只读访问或项目固件身份。

下一步保持一次只连接 `NUCLEO-02`、无 12 V、无外设，在默认参数下执行一次 `Connect`，只读取 Target information、`0x08000000` 起始内存和连接日志，随后正常 `Disconnect`。不得升级 ST-LINK、擦除、下载或写 Option Bytes。

## 5. 目标只读连接结果

| 字段 | 证据值 | 判定 |
|---|---|---|
| Connection state | `Connected` | PASS |
| STM32CubeProgrammer API | `v2.19.0 / Windows-64Bits` | 已记录 |
| 完整 ST-LINK SN | `066DFF515149856767254421` | 与 NUCLEO-01 的完整 SN 不同 |
| ST-LINK FW | `V2J46M32` | 与探针面板一致；未升级 |
| Voltage | 日志 `3.25 V`；面板 `3.24 V` | 无明显异常 |
| SWD / Connect / Reset | `4000 KHz / Normal / Software reset` | 与默认参数一致 |
| Device | `STM32F101/F102/F103 Medium-density` | 与 NUCLEO-F103RB 所属产品线一致 |
| Device ID / Revision | `0x410 / Rev X` | 已记录 |
| Flash / CPU | `128 KB / Cortex-M3` | 与目标基线一致 |
| Flash read | `0x08000000` 起始、`1024 Bytes`、`Data read successfully` | 只读访问 PASS |

日志中的 `UPLOADING OPTION BYTES DATA` 和 `UPLOADING` 表示 CubeProgrammer 从目标读取数据，不是向目标烧写。截图未显示 Erase、Download 或 Option Bytes 写操作。`Debug in Low Power mode is not supported for this device.` 仅说明低功耗调试选项不受支持；低功耗调试不在 H1 范围内，常规 SWD 连接和读取已经成功。

## 6. 正常断开与结论

断开截图同时显示：

- 顶部状态为 `Not connected`；
- 操作按钮恢复为 `Connect`；
- Target information 清空；
- 日志记录 `18:40:51 : Disconnected from device.`。

`NUCLEO-02` 单板 H1 结论：**PASS**。结合 `NUCLEO-01` 已完成的独立 PASS，Gate H1 判定为 **PASS**。该结论只覆盖两块 NUCLEO 的独立 USB/ST-LINK/VCP、稳定性、目标识别、Flash 只读访问和正常断开；不证明项目固件、CAN、FreeRTOS、Fault 或任何外设功能已经运行。
