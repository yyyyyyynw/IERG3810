# IERG3810 Labs

Experiment sources for **IERG3810** (2025–26 Term 1). Each `Exp*.c` file in `User/` is a standalone program with its own `main()`; the shared board, CMSIS and StdPeriph code lives in `Board/`, `Core/` and `Fw_lib/`.

| Lab | Topic | Experiments |
| --- | --- | --- |
| 1 | GPIO — LEDs, bit-banding, register-level port configuration | `Exp1.1`–`Exp1.5` |
| 2 | USART — clock tree and polling serial I/O on USART1/USART2 | `Exp2.1`–`Exp2.4` |
| 3 | FSMC + TFT LCD — 240×320 display, colours, text and images | `Exp3.1`–`Exp3.5` |
| 4 | Interrupts — EXTI, NVIC, keyboard input and LED control | `Exp4.1`–`Exp4.5` |
| 5 | Timers — TIM3/TIM4, SysTick, PWM, RGB LED and heartbeat scheduling | `Exp5.1`–`Exp5.7` |

## Build

Open `st12345.uvprojx` in Keil MDK-ARM (uVision 5), add the experiment you want to run to the build, exclude the other `Exp*.c` files, then build and download to the STM32F103ZE board.

See the [repository overview](../README.md) for the full lab and Snake mini-project description.
