# 双节点外设基线配置与编译验证记录

## 1. 记录目的

本记录说明正式 Node A 与 Node B STM32CubeMX 工程已经完成外设基线配置、代码生成和 STM32CubeIDE 编译验证。结论是两个工程均可在固定的 STM32CubeF1 1.8.7 环境下独立生成并完成编译链接，能够作为后续 FreeRTOS、驱动和通信实现的可回退基线。

本记录不改变 SAD、Fault List、CAN Matrix 或 FreeRTOS Task Architecture 已冻结的项目范围。

## 2. 基线信息

| 项目 | Node A | Node B |
|---|---|---|
| MCU 与开发板 | STM32F103RBT6 / NUCLEO-F103RB | STM32F103RBT6 / NUCLEO-F103RB |
| CubeMX | 6.18.1 | 6.18.1 |
| MCU 固件包 | STM32CubeF1 1.8.7，固定版本 | STM32CubeF1 1.8.7，固定版本 |
| 工具链 | STM32CubeIDE | STM32CubeIDE |
| 工程结构 | Basic，`Inc` / `Src`，按外设生成 `.c/.h` | Basic，`Inc` / `Src`，按外设生成 `.c/.h` |
| Git 基线提交 | `9d4d0ea` | `88f3d57` |

## 3. 公共配置

两个节点均采用 HSI 8 MHz 经 HSI/2 和 PLL x16 得到 64 MHz 系统时钟：HCLK 64 MHz、PCLK1 32 MHz、PCLK2 64 MHz。CAN1 使用 PA11/PA12，配置为 500 kbit/s，Prescaler 4、BS1 13 TQ、BS2 2 TQ、SJW 1 TQ，CAN RX FIFO0 中断抢占优先级为 5。USART2 使用 PA2/PA3，配置为 115200 bit/s、8-N-1、无硬件流控；生成代码中 PA2 为复用推挽输出，PA3 为无上下拉输入。

两个节点均保留 SWD。板卡模板同时带入 PA5 板载 LED 和 PC13 板载按键；PC13 当前仍按 EXTI 下降沿和优先级 0 生成，必须在启用 FreeRTOS 前统一决定改为轮询输入或重新分配其中断优先级。

## 4. Node A 外设基线

| 功能 | 配置结果 |
|---|---|
| 风扇 PWM | TIM3_CH2 / PC7，Full Remap，开漏复用输出，PSC 0，ARR 2559，配置频率 25 kHz |
| 风扇 Tach | TIM2_CH1 / PA0，下降沿输入捕获，PSC 639，ARR 65535，计数频率 100 kHz，中断优先级 6 |
| 电流与电压监测接口 | I2C1 Remap，PB8/PB9，100 kHz，标准模式 |
| INA260 ALERT | PB5 入口保留，尚未启用 |

Node A 生成代码已核对时钟、CAN、USART2、I2C1、TIM2 和 TIM3 初始化。当前 PWM、输入捕获、I2C 事务和 CAN 收发尚未在应用代码中启动。

## 5. Node B 外设基线

| 功能 | 配置结果 |
|---|---|
| NTC 模拟输入 | ADC1_IN0 / PA0，单通道 Rank 1，右对齐，软件触发 |
| ADC 采样 | Continuous 和 Scan 均关闭，不使用 DMA 或 ADC 中断，Sampling Time 55.5 cycles |
| ADC 时钟 | PCLK2 / 6，约 10.67 MHz，低于 STM32F103 的 14 MHz 上限 |

Node B 生成代码已核对 ADC 时钟分频、PA0 模拟模式、通道与采样时间、CAN 位时序和 USART2 GPIO。ADC 校准和转换启动尚未加入应用代码。

## 6. 编译结果

| 节点 | text | data | bss | 估算 Flash `text + data` | 静态 RAM `data + bss` | 结果 |
|---|---:|---:|---:|---:|---:|---|
| Node A | 13,432 B | 12 B | 1,908 B | 13,444 B | 1,920 B | 0 errors，0 warnings |
| Node B | 9,188 B | 12 B | 1,732 B | 9,200 B | 1,744 B | 0 errors，0 warnings |

`arm-none-eabi-size` 的 `dec` 是 text、data 和 bss 的算术和，不应作为单一 Flash 或 RAM 占用解释。当前数值只用于记录外设空工程的软件基线，后续加入 FreeRTOS 和业务模块后会增长。

## 7. 已验证与未验证边界

本阶段已经验证：CubeMX 配置能够保存并生成代码；生成工程能被 CubeIDE 导入；HAL、CMSIS、启动文件和链接脚本能够完成编译链接；关键外设初始化代码与 `.ioc` 配置一致；`Debug` 构建目录和编译产物未进入 Git。

本阶段尚未验证：开发板下载与运行、两节点 CAN 实际通信、CAN 过滤器及启动流程、ADC 校准与 NTC 采样、PWM 实际波形、Tach 捕获、INA260 通信、FreeRTOS 调度、Fault Detection、Fallback、Recovery 和完整硬件闭环。因此本记录不得被引用为硬件功能或实时性能已经实测的证据。

## 8. 下一阶段入口

下一阶段按冻结的 FreeRTOS Task Architecture 进行实现配置：Node A 建立 10 ms 控制任务、事件触发的 CAN 接收任务和 100 ms 周期任务；Node B 建立事件触发的 CAN 接收任务和 100 ms 监管任务；两个 CAN RX FIFO Queue 深度均为 4，latest-state Mailbox 使用长度为 1 的 Queue。启用 FreeRTOS 时将 HAL Tick 从 SysTick 迁移到 TIM4，并在生成代码前统一检查全部中断优先级。
