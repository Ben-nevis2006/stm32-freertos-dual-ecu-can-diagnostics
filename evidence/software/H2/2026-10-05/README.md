# Gate H2 最小 CAN 测试固件构建证据（2026-10-05）

> **历史快照说明（统一推送保留）**：本文件如实记录 2026-10-05 的构建成功与首次实板运行失败，当时的 `PENDING/HOLD` 不作回填。后续 2026-10-07 完成多故障定位、正式帧、断线注入和恢复验收，最终 H2 PASS 见 [`2026-10-07 证据索引`](../2026-10-07/README.md)。两份记录同时存在，用于体现推进过程而非只保留最终成功。

## 结论

Node A 与 Node B 的 H2 最小双向 CAN 测试固件均完成本地静默全量构建，退出码为 `0`，构建输出未出现警告或错误。2026-10-05 随后按 ST-LINK 序列号分别完成写入、校验与复位；但首次约 `15 s` 双串口运行预检**未通过 CAN 收发条件**：两端 MCU/RTOS/UART 均运行，两端 CAN 均仅成功排入最初 3 帧，此后硬件发送邮箱不释放且接收计数保持 0。正式 `60 s` 测试因此未启动。

测试协议见 [H2 CAN Minimal Test Protocol / H2 CAN 最小测试协议](../../../../current/05_verification_gates_验证与门禁/H2_CAN_Minimal_Test_Protocol_H2_CAN最小测试协议_2026-10-05.md)。

## 源码与工具基线

| 项目 | 内容 |
|---|---|
| 工作树基线提交 | `e4b9b379b34aebc141bb1ff7daab50cb4b2266fd`（H2 阶段修改尚未提交） |
| IDE/工程生成环境 | STM32CubeIDE 2.2.0 工程 |
| 编译器 | GNU Tools for STM32 `14.3.1 20250623`（发行标识 `14.3.rel1.20251027-0700`） |
| Make | GNU Make `4.4.1_st_20260330-0700` |
| 配置 | 两工程 `Debug`，`-Wall`、`-fstack-usage`、`-fcyclomatic-complexity` |

本次涉及的受控源码为：

- `firmware/node_a/Src/can.c`
- `firmware/node_a/Src/freertos.c`
- `firmware/node_a/node_a.ioc`
- `firmware/node_b/Src/can.c`
- `firmware/node_b/Src/freertos.c`
- `firmware/node_b/node_b.ioc`

## 构建产物

| 节点 | ELF | text | data | bss | dec | 文件字节数 | SHA-256 |
|---|---|---:|---:|---:|---:|---:|---|
| Node A | `firmware/node_a/Debug/node_a.elf` | 33076 | 96 | 14008 | 47180 | 1096172 | `E242964BA5F7009DDBA90A68FD1538750014FAFA694E6527D2A6DE8C4497B300` |
| Node B | `firmware/node_b/Debug/node_b.elf` | 29680 | 96 | 13824 | 43600 | 1078672 | `CA74BE27C8986A340E5F7E46454FB94AB0F2290BDA7111440314F6E1092D54F3` |

`Debug` 产物按仓库规则保持为本地生成文件；本次已按上表产物及既定 ST-LINK 序列号定向烧录，Node A/Node B 两次均由 CubeProgrammer 返回 `Download verified successfully`。以后重新构建再烧录时仍须重新计算哈希，不能沿用本表假定。

## 实板刷写与首次运行预检

| 项目 | Node A | Node B |
|---|---|---|
| ST-LINK 序列号 | `066BFF575151676667043206` | `066DFF515149856767254421` |
| 当次 VCP | `COM7` | `COM8` |
| 刷写目标 | `firmware/node_a/Debug/node_a.elf` | `firmware/node_b/Debug/node_b.elf` |
| CubeProgrammer | 写入、校验、复位成功 | 写入、校验、复位成功 |
| 约 15 s 预检 | `tx_q=3` 后邮箱堵满；`rx=0` | `tx_q=3` 后邮箱堵满；`rx=0` |

两端典型日志及字段解释见 [`H2_CAN_runtime-precheck_failed_20261005.txt`](H2_CAN_runtime-precheck_failed_20261005.txt)。这次失败不是“程序没有跑”：串口状态行持续输出，且计数器递增；它证明的是当时的 CAN 物理信号链没有完成一次可确认传输。

失败后按 ST UM1724 Figure 13/Table 27 与现场实物重新核对。两块板的信号线都位于正确的板边外侧偶数列；此前仅凭斜视照片得出的“内侧奇数列”判断已撤回。现场进一步确认：Node B 为棕线 `TX→CN10-12/PA12`、黄线 `RX→CN10-14/PA11`，接线正确；Node A 则两线对调，黄线在 PA12、棕线在 PA11。因此当前诊断优先级为：全断电只交换 Node A 的棕/黄线并重新拍照复核，再重跑同一固件；在复测前不把 HSI 频差、收发器焊点或 CAN 位时序列为已证实根因。

## 静态核对

- 两份 ELF 均存在强定义 `HAL_CAN_RxFifo0MsgPendingCallback`，并包含各自任务入口。
- Node A ELF 包含 `H2,A,BOOT`、`H2,A,BOOT_FAIL`、`H2,A,STAT`；Node B ELF 包含对应 `H2,B,*` 字符串。
- `H2_LogBoot` 静态栈占用 `208 B`，`H2_LogStatus` 为 `376 B`，CAN FIFO0 回调为 `72 B`；承担格式化日志的任务栈已设为 `2048 B`。
- Node A 任务栈合计 `4096 B`，Node B 任务栈合计 `3072 B`；两工程 `configTOTAL_HEAP_SIZE=8192`，并启用 `configCHECK_FOR_STACK_OVERFLOW=2` 与 malloc 失败钩子。
- `git diff --check` 退出码为 `0`；仅报告工作树 LF/CRLF 转换提示，无空白错误。
- CAN 名义位时序为 APB1 `32 MHz`、Prescaler `4`、BS1 `13 TQ`、BS2 `2 TQ`、SJW `1 TQ`，计算结果 `500 kbit/s`、采样点 `87.5%`。

## 证据边界与待办

本记录仍不能证明：

- 两块 CAN Pal 的 TX/RX 焊点在动态条件下可靠导通；
- 两节点 HSI 实际频差满足 CAN 运行要求；
- `0x321/0x322` 双向帧、UART 状态字段或错误恢复在实板上正常；
- 正式 `0x100/0x180` 协议、断开一端故障观察或 Gate H2 已完成。

下一证据必须来自 Node A 棕/黄线断电互换后的 CN10 近照，以及随后双 VCP 的短时复测日志；复测应确认 `tx_q` 持续增加、`rx>0`、`mb_free` 能恢复，之后才进行正式 `60 s` 采集。H2 尚未关闭，因此本轮未提交、未推送 GitHub。
