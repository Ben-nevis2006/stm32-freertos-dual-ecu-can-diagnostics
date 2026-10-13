# H3 Node A FAN-02B 刷写与运行身份证据（2026-10-13）

## 1. 日期与范围

本目录续接 2026-10-12 生成的 FAN-02B 受控占空比镜像。构建产物实际时间为 `2026-10-12 20:33:14`；定向刷写、读回校验、软件复位和串口身份捕获发生于 `2026-10-13`。两个日期分别保留，用于体现项目跨日推进，而非把构建与上板压缩为同一事件。

本目录只关闭 **目标身份 + 镜像不可变性 + 刷写 + 校验 + 复位 + 运行身份**。五档动态波形和真实风扇响应仍由后续硬件证据关闭。

## 2. 证据索引

| 文件 | 大小 | SHA-256 | 支持事实 |
|---|---:|---|---|
| [`H3_NODE-A_FAN-02B_profile-flash-identity_20261013.txt`](H3_NODE-A_FAN-02B_profile-flash-identity_20261013.txt) | `4138` bytes | `2B451892D3791F768423FF8CFAC10D7F39745AD21D0F2AE56701910001EA6449` | ST-Link/VCP 身份、ELF 哈希、下载与读回校验、复位、COM7 镜像身份和空载短时状态 |

## 3. 关键结果

- 唯一目标 ST-Link：`066BFF575151676667043206`，VCP=`COM7`；
- 目标：`3.23～3.24 V / Device ID 0x410 / Rev X / 128 KiB / Cortex-M3`；
- ELF：`1099336` bytes，SHA-256=`F499425FBF1D7931FF2BB6A8FDE3CF87A4D3BD8C2002BEA670C938D88CF0D2A1`，与 2026-10-12 构建证据一致；
- STM32CubeProgrammer：`File download complete`、`Download verified successfully`、软件复位、退出码 `0`；
- COM7 回读命中 `PC7/TIM3_CH2/25000 Hz/idle 50%/100-50-25-0-100/10 s/B1/AF_OD` 契约；
- 捕获的 7 条空载 `STAT` 均为 `hal_err=0x00000000`、`esr=0x00000000`。

受限执行环境最初无法打开 WinUSB，虽返回 `No ST-Link detected` / `DEV_CONNECT_ERR`，Windows PnP 同时显示调试接口正常。改为在受限环境外只读枚举后立即获得冻结序列号，因此该现象记录为工具执行边界，不作为板卡故障；失败尝试没有写入目标。

## 4. 判定

`FAN-02B` 的定向刷写与运行身份判定为 **PASS**。尚未关闭 B1 触发、各档实际波形、10 s 档位时序、结束恢复 50% 和真实风扇响应。
