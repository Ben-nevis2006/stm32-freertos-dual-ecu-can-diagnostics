# Gate H1 现场证据索引（2026-10-01）

## 1. 本批证据目的

本目录保存 `NUCLEO-02` 的 STM32CubeProgrammer ST-LINK 探针发现截图。此前的上电前照片、Windows ST-LINK/VCP 枚举和 30 s 稳定性证据见 [2026-09-29 证据索引](../2026-09-29/README.md)。

## 2. 证据清单与 SHA-256

| 归档文件 | 原始文件名 | 字节数 | SHA-256 |
|---|---|---:|---|
| `H1_NUCLEO-02_cubeprogrammer-probe-panel_20261001_01.png` | `codex-clipboard-bbb13a8b-855c-4976-bc5b-ac08ef221ce7.png` | 61316 | `FF618DD38A9A1E6CC259E1B0729F06A5325D4DAC9EF6C9B12431422E9C77ECFF` |

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
