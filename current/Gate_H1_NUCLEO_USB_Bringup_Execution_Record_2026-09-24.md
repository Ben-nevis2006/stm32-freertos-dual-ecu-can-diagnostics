# Gate H1 单块 NUCLEO USB Bring-up 执行记录

## 1. 文档控制

| 项目 | 内容 |
|---|---|
| 项目 | 基于 STM32 + FreeRTOS 的双 ECU CAN 实时通信与故障检测系统 |
| Gate | H1：两块 NUCLEO-F103RB 的独立 USB Bring-up |
| 版本 | v0.7 |
| 日期 | 2026-09-29 |
| 状态 | **IN PROGRESS；NUCLEO-01 单板 PASS，NUCLEO-02 尚未执行** |
| 准入依据 | [Gate H0 v1.3：CONDITIONAL PASS](Gate_H0_Hardware_Acceptance_Baseline_2026-09-22.md) |
| 软件/IPC 功能基线 | `4377c5e feat: add dual-node FreeRTOS IPC baseline` |
| H1 现场执行开始前 Git HEAD | `646bb6c`；现场记录不改变固件内容 |
| 允许对象 | `NUCLEO-01`、`NUCLEO-02`，每次仅一块 |
| 输出 | 每块板的照片、Windows 枚举、ST-LINK/VCP、CubeProgrammer 只读识别记录和 H1 结论 |
| 首批现场证据 | [2026-09-27：NUCLEO-01 上电前与线缆接口确认原图](../evidence/hardware/H1/2026-09-27/README.md) |
| 探针发现证据 | [2026-09-29：CubeProgrammer ST-LINK 探针面板](../evidence/hardware/H1/2026-09-29/README.md) |

本记录启动 H1，但不把“文档已创建”误写成“硬件已验证”。只有两块板分别完成本文件的步骤并留下证据后，H1 才能关闭。

## 2. H1 目标与非目标

H1 只回答以下问题：

1. `NUCLEO-01` 和 `NUCLEO-02` 能否分别通过板载 ST-LINK USB 可靠枚举；
2. Windows 能否分别识别 ST-LINK 和虚拟串口；
3. STM32CubeProgrammer 能否通过 ST-LINK 连接并只读识别目标 MCU；
4. 两块板是否存在独立的供电、枚举、调试器或目标连接异常；
5. 是否具备进入后续最小固件运行验证的条件。

H1 不验证 CAN、FreeRTOS 任务、Fault、INA260、NTC、Fan、Heater、12 V 电源、双节点通信或完整功率回路。USB 枚举成功也不能证明这些功能正常。

## 3. H0 带入偏差与隔离边界

| 偏差 | H1 影响 | H1 处置 | 最迟关闭点 |
|---|---|---|---|
| D-02：部分背面、芯片顶标和外设身份追溯不足 | 不阻止两块已明确正面身份的 NUCLEO 做纯 USB 验证 | 不接扩展线、盾板、CAN Pal、INA260、NTC、Fan 或 Heater；保留两块板的独立 ID | NUCLEO 接外部信号前补板背面；相应外设接入前补对应证据 |
| D-04：12 V 适配器连续稳定性记录缺失 | 与 USB 单板 H1 解耦 | 适配器不接市电、不接板、不接负载；H1 全程不出现 12 V | 任何 12 V 使用前 |

上述隔离边界是 H1 放行条件，不是建议。越界即停止 H1，并将 Gate 状态退回 HOLD 评审。

## 4. 安全状态与禁止事项

H1 开始前必须同时满足：

- 桌面为干燥、无裸露金属的非导电工作面；
- 12 V 适配器、Fan、Heater、CAN Pal、INA260、NTC 和全部板间线移出操作区；
- 一次只拿出一块 NUCLEO，另一块保持断开；
- NUCLEO 上不插杜邦线、盾板或外部电源；
- 使用确认具备数据功能的 USB 线，优先直连电脑 USB 口；
- 不更改跳线、焊桥、BOOT 配置或 ST-LINK 隔离跳线；若现场状态与 H0 照片不一致，先停止并拍照；
- 不擦除、不下载、不写 Option Bytes、不升级 ST-LINK 固件；本 Gate 只做枚举和只读识别；
- 不修改项目代码、冻结 SAD、Fault List、CAN Matrix 或 FreeRTOS Task Architecture。

出现异味、异常发热、USB 反复掉线、冒烟、火花、插头明显松动或 Windows 持续报错时，立即拔掉电脑侧 USB，并把该板标记为 HOLD。不得通过反复插拔、改跳线或接 12 V“试试看”。

## 5. 所需工具与版本记录

| 项目 | 现场记录 |
|---|---|
| Windows 版本 | 待填 |
| STM32CubeProgrammer 版本 | `v2.19.0 / Windows-64Bits` |
| ST-LINK 驱动版本 | 待填；若无法直接读取，记录为“未读取” |
| USB 数据线标识 | `USBCABLE-01`；USB-A 至 USB Mini-B；已通过 ST-LINK/VCP 枚举确认具备数据能力 |
| 使用的电脑 USB 端口 | 待填 |
| 照片/截图时间 | 2026-09-27：H1-00、通电及 Windows 枚举；2026-09-29：CubeProgrammer 探针面板；目标连接截图待填 |

不应把“设备管理器里出现一个 COM 口”当成充分证据。必须把设备名称、COM 号、ST-LINK 连接结果和目标识别结果关联到同一块实体板。

## 6. 证据命名

建议保留以下原始证据，不覆盖、不修图：

```text
H1_NUCLEO-01_front-before-usb_YYYYMMDD_01.jpg
H1_NUCLEO-01_windows-enumeration_YYYYMMDD_01.png
H1_NUCLEO-01_cubeprogrammer-target_YYYYMMDD_01.png
H1_NUCLEO-02_front-before-usb_YYYYMMDD_01.jpg
H1_NUCLEO-02_windows-enumeration_YYYYMMDD_01.png
H1_NUCLEO-02_cubeprogrammer-target_YYYYMMDD_01.png
```

首批实物照片、通电照及 Windows 枚举截图已归档至 [evidence/hardware/H1/2026-09-27](../evidence/hardware/H1/2026-09-27/README.md)。其中第一张照片的线缆设备端拍摄角度存在歧义，第二张补拍照片已明确其为 USB Mini-B；两张均保留以形成可审计的判断更正链路。

截图必须能辨认板 ID 对应关系。最稳妥的方法是：每块板开始前先拍带手写 ID 的正面照，并在截图记录表中写明执行时间；不要同时把两块板插在电脑上。

## 7. 执行步骤

### H1-00 工作台复位

1. 关闭 STM32CubeProgrammer，拔除电脑上其他 ST-LINK/串口设备；
2. 确认 12 V 适配器未接市电且已移出操作区；
3. 移走所有外设模块和跳线，只保留电脑、USB 数据线和 `NUCLEO-01`；
4. 对照 H0 正面照片检查 `NUCLEO-01` 的板号、手写 ID、USB 接口和跳线状态；
5. 拍摄 `NUCLEO-01` 的上电前正面照；
6. 若发现机械损伤、松动、异物、跳线变化或身份不一致，停止并登记，不接 USB。

### H1-01 NUCLEO-01 独立 USB Bring-up

1. 确认板上所有 Arduino/Morpho 排针为空；
2. 将 USB 数据线插入板载 ST-LINK USB 接口，再连接电脑；
3. 观察 30 s，记录供电/通信 LED 是否有异常闪烁、异味或明显发热；不凭 LED 颜色单独判定 PASS；
4. 打开 Windows 设备管理器，记录 ST-LINK 相关设备名称和虚拟串口 COM 号，保存截图；
5. 等待 30 s，确认设备没有反复消失/重连；
6. 打开 STM32CubeProgrammer，选择 ST-LINK，刷新探针列表；
7. 记录 ST-LINK 序列号、固件版本、目标电压和连接状态；
8. 点击连接，只读记录目标器件名称/Device ID、Flash 容量等可见信息；
9. 不执行 Erase、Download、Option Bytes 写入或固件升级；
10. 断开 CubeProgrammer 连接，关闭软件，从电脑侧拔掉 USB；
11. 触摸检查仅限确认有无异常烫手，不进行任何功率或温升结论；
12. 填写 NUCLEO-01 结果表。

#### 2026-09-27 阶段性执行结果

- `NUCLEO-01` 已通过 `USBCABLE-01` 连接电脑，通电照片中未接入任何 12 V 或外设；
- 现场报告：`LD2` 约每秒闪烁一次；`LD1` 上电初期闪烁一段时间后转为常亮；
- Windows 设备管理器已枚举 `ST-Link Debug`；
- Windows 已枚举 `STMicroelectronics STLink Virtual COM Port (COM7)`；
- 因调试接口与 VCP 两个 USB 功能均已枚举，`USBCABLE-01` 的数据能力判定为 PASS；
- LED 行为只作为现场现象记录。`LD2` 周期闪烁可能来自板内既有程序，但在未读取目标、未核验固件来源前，不得据此声称本项目固件、FreeRTOS、CAN 或 Fault 功能已经运行；
- 执行人于 2026-09-27 明确确认：30 s 内无异味、无异常发热、无火花、无 Windows 反复断连；该安全与稳定性检查判定为 PASS；
- CubeProgrammer 的探针和目标 MCU 只读识别尚未执行。

#### 2026-09-29 CubeProgrammer 探针发现结果

- CubeProgrammer 已发现 ST-LINK，界面显示序列号前缀 `066BFF575…`；完整序列号因下拉框截断尚待连接日志取证；
- 连接参数保持为 `SWD / 4000 kHz / Normal / Access Port 0 / Software reset / Reliable / Shared Disabled`；
- 目标电压为 `3.24 V`；
- ST-LINK 固件版本为 `V2J46M32`；
- 未点击 `Firmware upgrade`，未点击 `Connect`，未对目标执行写入；
- 探针发现判定为 PASS，目标 MCU / Device ID 和 Flash 容量仍待只读连接确认。

#### 2026-09-29 CubeProgrammer 目标只读连接结果

- CubeProgrammer 显示 `Connected`；
- Target information 显示 Device 为 `STM32F101/F102/F103 Medium-density`、Type 为 `MCU`、Device ID 为 `0x410`、Revision ID 为 `Rev X`、Flash size 为 `128 KB`、CPU 为 `Cortex-M3`；
- 目标电压仍为 `3.24 V`，ST-LINK 固件版本仍为 `V2J46M32`；
- 软件从地址 `0x08000000` 成功读取 `1024 Bytes`。日志中的 `UPLOADING` 表示从目标读取到主机，本截图未显示 Erase、Download 或 Option Bytes 写操作；
- Device ID `0x410` 本身对应 STM32F101/F102/F103 Medium-density 产品线，结合板卡丝印、`128 KB` Flash 和 Cortex-M3 信息，与 `NUCLEO-F103RB` 基线一致；不应仅凭 Device ID 声称已验证具体封装顶标或项目固件身份；
- Board 字段未提供具体板型，不作为本 Gate 的失败项；
- 目标连接、器件族识别、Flash 容量和只读访问判定为 PASS；完整 ST-LINK 序列号与 STM32CubeProgrammer 软件版本仍待补证。

#### 2026-09-29 NUCLEO-01 补证与单板结论

- 连接日志记录完整 ST-LINK SN：`066BFF575151676667043206`；
- ST-LINK FW：`V2J46M32`；目标电压：`3.24 V`；SWD：`4000 KHz`；Connect mode：`Normal`；Reset mode：`Software reset`；
- Device ID：`0x410`；Revision ID：`Rev X`；
- CubeProgrammer 在连接时自动读取 Option Bytes 与 `0x08000000` 起始的 `1024 Bytes` Flash；日志中的 `UPLOADING` 是从目标到主机的读取，未显示写入；
- 日志提示 `Debug in Low Power mode is not supported for this device.`。低功耗调试不在 H1 范围内，且常规连接与读取成功，因此该提示不影响本 Gate 判定；
- `20:58:51` 日志记录 `Disconnected from device.`，证明软件连接已按流程断开；
- STM32CubeProgrammer About 界面记录软件版本 `v2.19.0`；连接日志同时记录 API `v2.19.0 / Windows-64Bits`；
- `NUCLEO-01` 单板结论：**PASS**。该 PASS 仅覆盖独立 USB 枚举、稳定性、ST-LINK/VCP、目标识别和只读访问，不覆盖项目固件身份、FreeRTOS、CAN、Fault 或外设功能。

### H1-02 NUCLEO-02 独立 USB Bring-up

确认 `NUCLEO-01` 已完全断开并移出操作区后，对 `NUCLEO-02` 独立重复 H1-00 和 H1-01。不得保留上一块板的 COM 号、序列号或截图作为本板证据。

### H1-03 双板结果核对

1. 比较两块板的 ST-LINK 序列号，必须各自记录，不能复制；
2. 比较 Windows 枚举稳定性、目标电压和 MCU 识别结果；
3. 任一板失败时，仅该板先标记 HOLD；另一块板的 PASS 不得覆盖失败；
4. H1 关闭前，不把 NUCLEO-01/02 永久映射为 Node A/Node B；映射应在两块板均通过并有独立序列号后形成受控记录。

## 8. 结果记录表

| 检查项 | NUCLEO-01 | NUCLEO-02 | 判定依据 |
|---|---|---|---|
| 上电前身份/外观 | PASS（2026-09-27 照片可见范围） | NOT EXECUTED | 板号、手写 ID、USB 接口、跳线与 H0 记录一致 |
| USB 插头机械规格 | PASS：USB-A 至 Mini-B | NOT EXECUTED | 插头与 CN1 形态匹配；不等于数据能力通过 |
| Windows ST-LINK 枚举 | PASS：`ST-Link Debug` | NOT EXECUTED | 设备稳定存在，无反复掉线 |
| 虚拟串口 | PASS：`STMicroelectronics STLink Virtual COM Port (COM7)` | NOT EXECUTED | 记录设备名和 COM 号 |
| ST-LINK 序列号 | `066BFF575151676667043206` | 待填 | 两块板分别记录 |
| ST-LINK 固件版本 | `V2J46M32` | 待填 | 只读记录 |
| CubeProgrammer 目标电压 | `3.24 V` | 待填 | 数值稳定、无明显异常；不在本 Gate 发明精密验收带宽 |
| 目标 MCU / Device ID | PASS：Medium-density STM32F101/102/103；`0x410`；`Rev X`；Cortex-M3 | 待填 | 与 NUCLEO-F103RB 基线一致；不一致则 HOLD |
| Flash 容量 | PASS：`128 KB` | 待填 | 与目标器件信息一致 |
| 目标 Flash 只读访问 | PASS：`0x08000000` 起始 `1024 Bytes` 读取成功 | NOT EXECUTED | 只读证据；不代表项目固件身份已验证 |
| 30 s 枚举稳定性 | PASS：执行人确认无反复断连 | NOT EXECUTED | 无反复断连 |
| 异味/异常发热 | PASS：执行人确认无异味、异常发热或火花 | NOT EXECUTED | 无异常 |
| 单板结论 | **PASS** | NOT EXECUTED | PASS / HOLD / FAIL |

## 9. Gate H1 判定

H1 只有在以下条件全部满足时才能 PASS：

1. 两块板均按独立顺序执行，证据可追溯到各自实体 ID；
2. 两块板均稳定枚举 ST-LINK 和虚拟串口；
3. CubeProgrammer 均能只读连接目标 MCU，目标身份与 NUCLEO-F103RB 一致；
4. 没有异常发热、异味、掉线或需要解释的目标电压异常；
5. 未更改跳线、未接外设/12 V、未擦除或下载固件；
6. 所有截图和读数均已归档，失败项没有被另一块板的成功结果覆盖。

当前 Gate H1 结论：**IN PROGRESS**。`NUCLEO-01` 已完成全部单板 H1 项并判定 PASS；`NUCLEO-02` 尚未执行，Gate H1 仍未关闭。`NUCLEO-01` 的成功不得代替或预判 `NUCLEO-02` 的结果。

H1 PASS 仍不自动授权 CAN、双节点、12 V、Fan、Heater、INA260、NTC 或故障注入。下一 Gate 的具体准入必须依据项目交接报告和冻结架构另行确认。

## 10. 第一现场动作

关闭 STM32CubeProgrammer，从电脑侧拔除 `NUCLEO-01` 的 USB，并将 `NUCLEO-01` 与 `USBCABLE-01` 分离后移出操作区。仅放入 `NUCLEO-02` 和同一条 `USBCABLE-01`，确认手写 ID `02`、排针为空、跳线状态无变化并拍摄上电前正面照；在照片复核前不接 USB。

## 11. 修订记录

| 版本 | 日期 | 修改内容 | 原因 | 作者/执行人 |
|---|---|---|---|---|
| v0.1 | 2026-09-24 | 建立 H1 单板 USB Bring-up 范围、安全边界、逐板步骤、证据与判定表 | Gate H0 已有条件关闭，D-02/D-04 可通过隔离与纯 USB H1 解耦 | Codex 整理；执行人待补签 |
| v0.2 | 2026-09-27 | 归档 NUCLEO-01 上电前照片与线缆补拍；更正初始角度导致的插头误判；放行 H1-00 并开启 H1-01 | 补拍已确认 USB-A 至 Mini-B 插头形态，板卡仍保持未连接、无外设状态 | Codex 整理；现场执行人：用户 |
| v0.3 | 2026-09-27 | 归档 NUCLEO-01 通电照、ST-Link Debug 与 VCP/COM7 枚举截图；记录 LD1/LD2 现场现象 | Windows 已同时识别调试接口和虚拟串口，USB 数据通路获得证据；目标 MCU 尚未只读识别 | Codex 整理；现场执行人：用户 |
| v0.4 | 2026-09-27 | 记录 30 s 稳定性及无异味、异常发热、火花的操作员确认；放行 CubeProgrammer 探针发现步骤 | 负向安全条件不能由静态照片推断，需现场执行人明确声明 | Codex 整理；现场执行人：用户 |
| v0.5 | 2026-09-29 | 归档 CubeProgrammer 探针面板；记录序列号前缀、连接参数、3.24 V 目标电压和 V2J46M32 固件版本 | 探针发现和供电读数已获得只读证据，允许在默认参数下执行一次目标连接 | Codex 整理；现场执行人：用户 |
| v0.6 | 2026-09-29 | 归档 CubeProgrammer 成功连接与目标信息截图；记录 Device ID、Revision、Flash、CPU 和 1024-Byte 只读结果 | 目标识别与 Flash 读取已形成证据，但完整探针序列号和软件版本仍需补齐 | Codex 整理；现场执行人：用户 |
| v0.7 | 2026-09-29 | 归档完整连接日志与 CubeProgrammer About 截图；记录完整 ST-LINK SN、软件版本、只读 Option Bytes 行为和正常断开 | NUCLEO-01 的身份、枚举、稳定性、探针、目标和只读取证已闭环，形成单板 PASS | Codex 整理；现场执行人：用户 |
