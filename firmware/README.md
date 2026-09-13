# Firmware

This directory contains the two formal STM32CubeMX/STM32CubeIDE projects:

- `node_a`: CAN actuator ECU with PWM, tachometer capture, INA260 I2C, and debug UART.
- `node_b`: CAN supervisory ECU with analog temperature input and debug UART.

The disposable `F103RB_SmokeTest` environment check is intentionally not copied here.

Repository rules:

- Track each `.ioc` file and the CubeIDE project metadata required to regenerate and build the project.
- Track generated source, HAL/CMSIS drivers, and later FreeRTOS middleware so the pinned CubeF1 1.8.7 baseline remains reproducible.
- Do not track `Debug`/`Release` directories or compiler/linker outputs.
- During CubeMX-managed development, place manual code inside preserved `USER CODE` sections unless the code is intentionally kept in a separate application module.
