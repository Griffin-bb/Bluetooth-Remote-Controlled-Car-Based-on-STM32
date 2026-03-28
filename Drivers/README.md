# Drivers

This directory must be populated with the STM32 HAL/CMSIS files before building.

## Option A — STM32CubeMX (recommended)

1. Open STM32CubeMX and create a new project for **STM32F103C8Tx**.
2. Enable **TIM3** (PWM Generation CH1+CH2) and **USART1** (Asynchronous).
3. Generate code with the **Makefile** toolchain.
4. Copy the generated `Drivers/` directory into this project root,
   replacing this directory.

## Option B — Manual download

Download **STM32Cube_FW_F1** from
<https://github.com/STMicroelectronics/STM32CubeF1> and copy:

```
STM32CubeF1/
├─ Drivers/
│    ├─ CMSIS/
│    │    ├─ Include/                      → Drivers/CMSIS/Include/
│    │    └─ Device/ST/STM32F1xx/
│    │         ├─ Include/                 → Drivers/CMSIS/Device/ST/STM32F1xx/Include/
│    │         └─ Source/Templates/gcc/
│    │              └─ startup_stm32f103xb.s
│    └─ STM32F1xx_HAL_Driver/
│         ├─ Inc/                          → Drivers/STM32F1xx_HAL_Driver/Inc/
│         └─ Src/                          → Drivers/STM32F1xx_HAL_Driver/Src/
```
