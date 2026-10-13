# H3 Node A Fan PWM 固件证据（2026-10-12）

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

## 6. 硬件波形交叉证据

随后的风扇/12 V 隔离实板测得到 `25.03 kHz / 40.02 µs / 50%`，照片快照屏显为 `25.01 kHz / 39.98 µs`，与本镜像参数和串口身份行一致。电平为外部 3.3 V 上拉下的 `Vmax=3.48 V`、`Vmin=-40～-80 mV` 观察量级，10 s 稳定。完整无源预检、照片、哈希及读数边界见 [`evidence/hardware/H3/2026-10-12`](../../../hardware/H3/2026-10-12/README.md)。

据此 `FAN-01` 整体判定为 **PASS（固件身份 + PC7 开漏空载波形）**。

## 7. FAN-02B 受控占空比镜像构建

在 `FAN-02A` 固定 50% 真实风扇联调通过并按规定顺序断电后，继续准备 `FAN-02B`。构建动作实际发生于 **2026-10-12 20:33（Asia/Shanghai）**，证据说明于 **2026-10-13** 续录；两个日期分别保留，不把跨日推进压缩成同一时点。

新镜像保持 `PC7/D9/TIM3_CH2 / 25 kHz / AF Open-Drain`、H2 CAN 任务和 `BOOT/STAT` 日志不变。启动后默认保持 50%，只有按下 Node A 蓝色 `B1 USER` 键才执行一次：

```text
100%（10 s）→ 50%（10 s）→ 25%（10 s）→ 0%（10 s）→ 100%（10 s）→ 回到 50% 保持
```

`ARR=2559`、周期计数为 `2560`，对应 `CCR2=2560 / 1280 / 640 / 0 / 2560`。按键中断只置请求标志，实际 CCR2 更新和串口记录均由 `A_ControlTask` 完成；执行期间的按键抖动或额外按压被丢弃。新增运行身份契约为：

```text
H3,A,FAN_PWM_PROFILE,...,sequence=100-50-25-0-100,hold_ms=10000,trigger=B1,...
H3,A,FAN_PWM_STEP,index=<1..5>,total=5,duty_pct=<...>,compare=<...>,hold_ms=10000
H3,A,FAN_PWM_PROFILE_DONE,final_pct=50
```

STM32CubeIDE 2.2.0 随附 GNU Tools for STM32 14.3.1 单线程 clean build 成功，未观察到警告或错误：`text/data/bss/dec=34264/96/14008/48368`；`node_a.elf` 大小 `1099336` bytes，SHA-256=`F499425FBF1D7931FF2BB6A8FDE3CF87A4D3BD8C2002BEA670C938D88CF0D2A1`。`A_ControlTask` 配置栈为 2048 bytes，新增路径的最大单函数静态栈记录为 248 bytes。

规范化构建摘要为 [`H3_NODE-A_FAN-02B_profile-build_20261012.txt`](H3_NODE-A_FAN-02B_profile-build_20261012.txt)，大小 `2622` bytes，SHA-256=`DF9BD0A79B57D7EB7EF73D2BAA0428B22D56ACA9F7DFE1E7F75AF9D59B0BD265`。该摘要明确不是原始 IDE 控制台转储。

本节只关闭 **源码实现 + 本地构建**，在 2026-10-12 构建时点的状态为 **PASS（build-only）**。该时点镜像尚未刷入 Node A；不证明 B1 触发、各档实际波形、风扇减速/停转/重启、USB 稳定性或 CAN 运行健康。刷写前仍要求风扇 12 V、PWM、Tach、Heater 和 INA260 全部与 Node A 隔离，仅接 Node A USB。

跨日续接结果：目标身份、镜像哈希、定向下载、读回校验、复位和 COM7 运行身份已于 2026-10-13 通过，详见 [`../2026-10-13/README.md`](../2026-10-13/README.md)。动态波形和真实风扇响应仍不由本构建证据代替。
