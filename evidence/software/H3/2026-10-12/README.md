# H3 FAN-01 Node A PWM 固件准备证据（2026-10-12）

## 1. 目的与边界

本记录只覆盖 `FAN-01` 的 Node A 离线固件准备：使 `PC7/D9/TIM3_CH2` 在复用开漏模式下输出约 `25 kHz / 50%` 的固定波形，以便在风扇与 12 V 完全隔离时先验证 MCU 低压侧。它不包含风扇 PWM 联调、Tach/PPR、INA260、Heater 或完整温控闭环。

## 2. 实现变更

- 保留 CubeMX 既有 `TIM3 Full Remap`、`TIM3_CH2`、`PC7`、`GPIO_MODE_AF_OD`、`PSC=0`、`ARR=2559`。
- 启动 `HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2)`。
- 设置 `CCR2=1280`；64 MHz 定时器时钟下计算为 `64 MHz / (2559+1) = 25 kHz`，高电平/释放时间为 `1280/2560 = 50%`。
- 保留 H2 CAN 任务与原有 `BOOT/STAT` 日志，新增镜像身份行：

```text
H3,A,FAN_PWM_PROBE,pin=PC7,timer=TIM3_CH2,freq_hz=25000,duty_pct=50,mode=af_od
```

## 3. 构建结果

| 项目 | 结果 |
|---|---|
| 工程 | `firmware/node_a` |
| 工具链 | STM32CubeIDE 2.2.0；GNU Tools for STM32 14.3.1 |
| 构建方式 | 生成的 Debug Makefile，`-j1 all` |
| 退出码 | `0` |
| 警告 / 错误 | `0 / 0` |
| `text / data / bss / dec` | `33692 / 96 / 14008 / 47796` bytes |
| ELF 大小 | `1096932` bytes |
| ELF SHA-256 | `F3924691E9E3950200226378AD05B71410948F0627F4171017C626AA43B3D496` |

`Debug` 产物按仓库规则保持为本地生成文件，不进入 Git。

## 4. 实板状态与下一准入点

构建后的首次 STM32CubeProgrammer 2.19.0 只读列举返回 `No ST-Link detected`；该时点只能判定为 **BUILD PASS / FLASH NOT STARTED**，没有把本地构建提前记为实板结果。

下一步只在以下现场状态下授权 Node A 定向刷写：

1. 风扇适配器已断开市电，直流残压 `<0.5 V`；
2. 风扇 Power/PWM/Tach 与 Node A 全部隔离；
3. Heater、INA260 不接；
4. 只连接身份冻结为 Node A 的 NUCLEO USB，必须按 ST-LINK 序列号 `066BFF575151676667043206` 定向识别和写入；
5. 刷写完成后，仍不接风扇；用外部 `10 kΩ` 将 `PC7/D9` 上拉到同一块 Node A 的 `3V3`，示波器地夹只接 Node A GND，再测量波形。

## 5. 定向刷写与运行身份回读

执行人确认现场只连 Node A USB、风扇与 12 V 保持隔离后，只读列举中只有以下 ST-LINK：

| 项目 | 回读 |
|---|---|
| ST-LINK SN | `066BFF575151676667043206` |
| VCP | `COM7` |
| Target voltage | `3.24 V` |
| Device | `0x410 / Rev X / STM32F101/F102/F103 Medium-density` |
| Flash / CPU | `128 KiB / Cortex-M3` |

镜像 SHA-256 再次核验为 `F3924691E9E3950200226378AD05B71410948F0627F4171017C626AA43B3D496`，随后按上述序列号定向写入。STM32CubeProgrammer 返回：

```text
File download complete
Download verified successfully
MCU Reset
Software reset is performed
```

复位后以 `115200 8N1` 连续捕获 COM7 约 4 s，回读包含：

```text
H2,A,BOOT,profile=formal_baseline,bitrate=500000,tx_id=0x180,tx_dlc=5,rx_id=0x100,rx_dlc=2,period_ms=100,start_delay_ms=10000
H3,A,FAN_PWM_PROBE,pin=PC7,timer=TIM3_CH2,freq_hz=25000,duty_pct=50,mode=af_od
H2,A,STAT,tick=521,...,tx_fail=0,...,q_drop=0,...,hal_err=0x00000000,esr=0x00000000
H2,A,STAT,tick=1534,...,tx_fail=0,...,q_drop=0,...,hal_err=0x00000000,esr=0x00000000
H2,A,STAT,tick=2547,...,tx_fail=0,...,q_drop=0,...,hal_err=0x00000000,esr=0x00000000
H2,A,STAT,tick=3560,...,tx_fail=0,...,q_drop=0,...,hal_err=0x00000000,esr=0x00000000
```

据此，Node A 目标身份、写入、校验、复位和 FAN-01 镜像运行身份均为 **PASS**。当前仍未实测 PC7 的实际电压、频率和占空比，因此不得提前关闭 `FAN-01`。
