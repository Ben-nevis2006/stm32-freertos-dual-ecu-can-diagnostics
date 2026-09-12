# 开发环境搭建与最小编译验证记录

## 1. 记录信息

| 项目 | 内容 |
|---|---|
| 项目名称 | 基于 STM32 + FreeRTOS 的双 ECU CAN 实时通信与故障检测系统 |
| 记录日期 | 2026-09-12 |
| 当前阶段 | 硬件到货前的软件环境搭建 |
| 记录性质 | 开发环境基线与可追溯验证 |
| 验收结论 | 通过 |

## 2. 本次工作目标

本次工作不实现 CAN、FreeRTOS 或传感器业务功能，仅验证以下基础链路是否可用：

1. STM32CubeMX 能够识别目标开发板并生成工程；
2. STM32CubeF1 固件包能够被正确调用；
3. STM32CubeIDE 能够导入并构建生成的工程；
4. ARM GCC 编译、链接及构建后处理工具能够正常运行。

通过最小工程先隔离开发环境问题，避免后续将工具链故障与外设配置、RTOS 或业务逻辑故障混淆。

## 3. 软件环境基线

| 组件 | 验证版本 | 状态 |
|---|---:|---|
| STM32CubeMX | 6.18.1 | 已更新并验证 |
| STM32CubeMX 器件数据库 | DB 6.0.181 | 已更新并验证 |
| STM32CubeF1 固件包 | 1.8.7 | 已安装并验证 |
| STM32CubeIDE | 2.2.0 | 已安装并验证 |
| STM32CubeProgrammer | 2.19.0 | 已安装，尚未进行硬件连接验证 |
| GNU Tools for STM32 | GCC 14.3.1 | 已通过实际编译验证 |

STM32CubeF1 1.8.7 中用于本项目的 HAL、CMSIS、FreeRTOS 以及 `STM32F103RB-Nucleo` 示例目录均已确认存在。

## 4. CubeMX 更新问题及处置

### 4.1 现象

首次在 CubeMX 内执行更新后，STM32CubeF1 1.8.7 已成功安装，但 CubeMX 主程序仍运行 6.12.1。

### 4.2 原因定位

原 CubeMX 安装在 Windows 系统级 `Program Files` 目录。固件包写入用户级 Repository，因此可以正常更新；CubeMX 主程序自更新需要管理员权限完成下载和下一次启动时的文件替换。

### 4.3 处置结果

以管理员权限重新执行 CubeMX 自更新并再次启动后，完成主程序替换。复检结果如下：

- CubeMX 内部软件版本：`MX.6.18.1`；
- 器件数据库版本：`DB.6.0.181`；
- 实际可执行文件版本：`6.18.1-RC2`；
- 可执行文件数字签名：STMicroelectronics，有效；
- 安装目录中的主程序、JRE、数据库、插件和帮助文件均已更新。

Windows“已安装的应用”中仍显示 6.12.1，这是旧安装器注册信息未随自更新刷新造成的显示差异。实际程序及 CubeMX 内部状态均为 6.18.1，不影响后续使用。

> 注：可执行文件属性中的 `RC2` 为该发布包的内部构建标签。本项目的软件基线按 CubeMX 自身识别的公开版本 `6.18.1` 记录。

## 5. 最小编译冒烟测试

### 5.1 工程配置

| 配置项 | 实际值 |
|---|---|
| 测试工程名称 | `F103RB_SmokeTest` |
| 目标开发板 | `NUCLEO-F103RB` |
| 目标 MCU | `STM32F103RBT6` |
| MCU 封装 | LQFP64 |
| CubeF1 固件包 | `STM32Cube FW_F1 V1.8.7` |
| 编译器/链接器 | GCC |
| 目标 IDE | STM32CubeIDE |
| IDE 工程生成位置 | 工程根目录（Generate Under Root） |
| 保留用户代码 | 已启用 |
| 当前系统时钟 | 64 MHz |

该测试工程仅启用了生成工程所需的 NVIC、RCC、SYS 及板级 GPIO 初始化，没有加入 CAN、FreeRTOS 和项目业务外设。当前 64 MHz 时钟是本次板卡模板生成结果，只服务于环境验证，**不构成正式 Node A/Node B 的时钟架构决策**。

测试工程保存在独立的纯英文路径下，未纳入本仓库。它是可丢弃的环境验证样例，不作为正式固件工程继续开发。

### 5.2 构建结果

STM32CubeIDE 构建成功，并生成以下主要产物：

- `F103RB_SmokeTest.elf`；
- `F103RB_SmokeTest.map`；
- `F103RB_SmokeTest.list`。

构建结果：

```text
   text    data     bss     dec     hex  filename
   4796      12    1572    6380    18ec  F103RB_SmokeTest.elf

Build Finished. 0 errors, 0 warnings. (took 2s.758ms)
```

### 5.3 静态内存占用解释

以 STM32F103RB 的 128 KiB Flash 和 20 KiB SRAM 为基准：

- 静态 Flash 占用约为 `text + data = 4808 B`，约 3.7%；
- 静态 RAM 占用约为 `data + bss = 1584 B`，约 7.7%。

其中 `dec = 6380` 只是 `text`、`data`、`bss` 的算术合计，不能作为单一物理存储器的占用量。实际项目还需要结合链接 Map 文件、任务栈、堆以及运行期峰值继续评估内存余量。

## 6. 已验证范围

- CubeMX 6.18.1 能够识别 NUCLEO-F103RB；
- CubeF1 1.8.7 能够参与代码生成；
- CubeMX 到 CubeIDE 的工程交接正常；
- ARM GCC 编译和链接正常；
- `arm-none-eabi-size` 与 `arm-none-eabi-objdump` 后处理正常；
- 最小工程达到 0 errors、0 warnings。

## 7. 尚未验证范围

由于硬件尚未到货，以下内容不属于本次验收结论：

- NUCLEO-F103RB 的 USB 枚举与供电；
- ST-LINK 驱动、SWD 下载及断点调试；
- 实际系统时钟和时钟容差；
- 双节点 CAN 收发与物理层终端；
- FreeRTOS 任务调度和运行期栈水位；
- 风扇 PWM、转速捕获、INA260、NTC ADC；
- 故障注入、检测、降级和恢复机制；
- 实时性、总线负载及故障响应时间。

## 8. 阶段结论

软件环境搭建与最小编译链路验证完成，当前工具版本可以作为后续固件开发基线。

本次结果只证明“配置生成—工程导入—编译链接”链路可用，不代表双 ECU 系统功能已经实现，也不代表硬件链路已经验证。

## 9. 下一步计划

1. 在 CubeMX 正式建项前完成双 ECU 引脚、定时器、DMA、中断和时钟资源分配；
2. 明确 Node A 的 CAN、25 kHz 风扇 PWM、转速输入捕获和 INA260 I2C 资源；
3. 明确 Node B 的 CAN 和 NTC ADC 资源；
4. 固化正式时钟树及中断优先级约束；
5. 分别创建 Node A、Node B 的正式 `.ioc` 和可编译空壳工程；
6. 硬件到货后按“供电与枚举—下载调试—单外设—双节点 CAN—RTOS—故障机制”的顺序进行分阶段 Bring-up。

