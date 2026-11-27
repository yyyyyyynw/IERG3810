# IERG3810 — Microcontrollers and Embedded Systems Laboratory

Coursework for **IERG3810** (2025–26 Term 1) at The Chinese University of Hong Kong.
This repository contains the five hardware lab exercises and the **Snake** mini-project, written in C for the **STM32F103ZE** (ARM Cortex-M3) and built with Keil MDK and the STM32F10x Standard Peripheral Library.

## Repository layout

```
lab/       Lab 1–5 experiment sources and the shared Keil project
project/   Snake mini-project (board drivers + game logic)
```

- `lab/User/Exp*.c` — one standalone `main()` per experiment.
- `lab/Board/` — board support drivers (LCD, buzzer, LED, keys, USART, clock).
- `lab/Core/`, `lab/Fw_lib/` — CMSIS startup code and the STM32F10x StdPeriph driver library.
- `project/Board/` — game rendering/logic plus the TFT LCD, JoyPad, buzzer and LED drivers.
- `project/User/test.c` — game state machine, timer and JoyPad polling.

## Labs

| Lab | Topic | Experiments |
| --- | --- | --- |
| 1 | GPIO — LEDs, bit-banding, register-level port configuration | `Exp1.1`–`Exp1.5` |
| 2 | USART — clock tree, polling serial I/O on USART1/USART2 | `Exp2.1`–`Exp2.4` |
| 3 | FSMC + TFT LCD — 240×320 display, colours, text and images | `Exp3.1`–`Exp3.5` |
| 4 | Interrupts — EXTI, NVIC, keyboard input and LED control | `Exp4.1`–`Exp4.5` |
| 5 | Timers — TIM3/TIM4, SysTick, PWM, RGB LED and heartbeat scheduling | `Exp5.1`–`Exp5.7` |

The labs share a single Keil project: `lab/st12345.uvprojx`. Add the desired `Exp*.c` file to the build and exclude the others before downloading.

## Mini-project: Snake

A complete Snake game on the course STM32F103ZE board with a 240×320 TFT LCD and JoyPad.

- 15×15 playfield rendered cell by cell, with pixel-art sprites for the apple and scenery.
- Six modes: five hand-tuned levels plus an endless **Infinity Mode** that keeps raising the speed.
- Obstacles (blue fire), poison (purple), and stones that start moving from Level 4.
- Persistent high score, animated starfield title screen, and buzzer/LED feedback for eating, crashing and winning.

| Level | Walls | Stones | Moving stones | Poison | Apples to win |
| --- | --- | --- | --- | --- | --- |
| 1 | – | – | – | – | 5 |
| 2 | ✓ | – | – | – | 8 |
| 3 | ✓ | 3 | – | – | 10 |
| 4 | ✓ | 4 | ✓ | – | 12 |
| 5 | ✓ | 5 | ✓ | ✓ | 20 |
| Infinity | – | scales | ✓ | ✓ | endless |

### Controls

| Input | Action |
| --- | --- |
| D-Pad | Move the snake; navigate menus |
| START / SELECT | Confirm, continue, or restart |

The Keil project is `project/Snake.uvprojx`.

## Build and run

1. Open the relevant `.uvprojx` file in **Keil MDK-ARM (uVision 5)** with STM32F10x device support installed.
2. Build the project (`F7`).
3. Flash the board with your usual STM32 programmer (ST-Link, J-Link, or serial ISP) and reset.

The code targets the STM32F10x Standard Peripheral Library (v3.5.0) and does not use HAL or CubeMX.

## Notes

- Lab reports and submission archives are intentionally excluded from the repository via `.gitignore`.
- This is coursework code, shared for reference and portfolio purposes.
