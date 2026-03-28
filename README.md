# Bluetooth Remote-Controlled Car Based on STM32

A bare-metal firmware (v1.0.0) for driving a two-motor RC car with an Android
phone (or any Bluetooth serial terminal) over HC-05/HC-06.  Written in C11,
targeting the **STM32F103C8T6** ("Blue Pill") at 72 MHz.

---

## Hardware

| Component | Part | Notes |
|-----------|------|-------|
| MCU board | STM32F103C8T6 ("Blue Pill") | 72 MHz Cortex-M3 |
| Bluetooth | HC-05 or HC-06 | default baud 9600 |
| Motor driver | L298N dual H-bridge | 5–35 V motor supply |
| Motors | 2 × DC gear motor | e.g. TT motors |
| Power | 7.4 V 2S LiPo or 4 × AA | regulated 5 V for logic |

### Wiring

```
STM32 Blue-Pill          HC-05/HC-06
─────────────────────    ───────────
PA9  (USART1 TX) ──────► RX
PA10 (USART1 RX) ◄────── TX
3.3 V            ──────► VCC  (HC-06) / KEY pulled low for HC-05
GND              ──────── GND

STM32 Blue-Pill          L298N
─────────────────────    ──────────────────────────
PA6  (TIM3_CH1 PWM) ──► ENA  (left motor speed)
PA7  (TIM3_CH2 PWM) ──► ENB  (right motor speed)
PB0  (GPIO out)     ──► IN1  (left  forward)
PB1  (GPIO out)     ──► IN2  (left  reverse)
PB10 (GPIO out)     ──► IN3  (right forward)
PB11 (GPIO out)     ──► IN4  (right reverse)
5 V                 ──► VSS  (logic supply)
GND                 ──── GND
```

---

## Project structure

```
├── Inc/
│   ├── main.h                  Top-level header; peripheral handle externs
│   ├── motor.h                 DC motor control API
│   ├── bluetooth.h             Bluetooth command-parsing API
│   └── stm32f1xx_hal_conf.h    HAL module selection
├── Src/
│   ├── main.c                  System init + main loop
│   ├── motor.c                 L298N driver (PWM + GPIO)
│   ├── bluetooth.c             USART1 interrupt-driven reception
│   └── stm32f1xx_it.c          Cortex-M3 exception / ISR handlers
├── Drivers/                    ← populate from STM32CubeMX / STM32Cube_FW_F1
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
├── STM32F103C8TX_FLASH.ld      Linker script (64 KB flash / 20 KB RAM)
└── Makefile
```

---

## Bluetooth command set

| Byte received | Action |
|---------------|--------|
| `F` or `1`    | Forward |
| `B` or `2`    | Backward |
| `L` or `3`    | Turn left |
| `R` or `4`    | Turn right |
| `S` or `0`    | Stop |
| `5`–`9`       | Set speed to 50 %–90 % (persists until changed) |

Commands are case-insensitive (`f` == `F`).  The default speed at power-on is **75 %**.

---

## Build

### Prerequisites

* [arm-none-eabi-gcc](https://developer.arm.com/downloads/-/gnu-rm) ≥ 10.x
* STM32Cube_FW_F1 HAL/CMSIS drivers — copy into `Drivers/` (see structure above), or generate via [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)

### Compile

```bash
make
```

The build output is placed in `build/`:

| File | Description |
|------|-------------|
| `build/btcar.elf` | ELF with debug symbols |
| `build/btcar.bin` | Raw binary for flashing |
| `build/btcar.hex` | Intel HEX for ST-Link Utility |

### Flash (ST-Link + OpenOCD)

```bash
make flash
```

Or use **STM32CubeProgrammer** / **ST-Link Utility** with `build/btcar.hex`.

---

## License

MIT — see [LICENSE](LICENSE) for details.
