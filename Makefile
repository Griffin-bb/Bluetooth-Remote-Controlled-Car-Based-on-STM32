##############################################################################
# Makefile — Bluetooth Remote-Controlled Car (STM32F103C8T6)
#
# Prerequisites
# -------------
#   arm-none-eabi-gcc toolchain  (≥ 10.x recommended)
#   STM32CubeMX-generated HAL/CMSIS files placed in Drivers/
#     └─ Drivers/
#          ├─ CMSIS/
#          │    ├─ Include/               (core_cm3.h, cmsis_gcc.h, …)
#          │    └─ Device/ST/STM32F1xx/
#          │         ├─ Include/          (stm32f103xb.h, stm32f1xx.h, …)
#          │         └─ Source/Templates/
#          │              └─ gcc/
#          │                   └─ startup_stm32f103xb.s
#          └─ STM32F1xx_HAL_Driver/
#               ├─ Inc/                  (stm32f1xx_hal*.h)
#               └─ Src/                  (stm32f1xx_hal*.c)
#
# Targets
# -------
#   make         — build firmware (ELF + BIN + HEX)
#   make flash   — flash via ST-Link using OpenOCD
#   make clean   — remove build artefacts
##############################################################################

TARGET  := btcar
BUILD   := build
DEVICE  := STM32F103xB

# ── Toolchain ───────────────────────────────────────────────────────────────
PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
AS      := $(PREFIX)gcc -x assembler-with-cpp
CP      := $(PREFIX)objcopy
SZ      := $(PREFIX)size

# ── CPU / FPU flags ─────────────────────────────────────────────────────────
CPU     := -mcpu=cortex-m3
FPU     :=
FLOAT   := -mfloat-abi=soft
MCU     := $(CPU) -mthumb $(FPU) $(FLOAT)

# ── C compiler flags ────────────────────────────────────────────────────────
OPT     := -O2

CFLAGS  := $(MCU) $(OPT) -Wall -Wextra -Wno-unused-parameter \
           -fdata-sections -ffunction-sections \
           -D$(DEVICE) -DUSE_HAL_DRIVER \
           -IInc \
           -IDrivers/STM32F1xx_HAL_Driver/Inc \
           -IDrivers/CMSIS/Device/ST/STM32F1xx/Include \
           -IDrivers/CMSIS/Include \
           -std=c11

ASFLAGS := $(MCU) $(OPT) -Wall -fdata-sections -ffunction-sections

# ── Linker flags ────────────────────────────────────────────────────────────
LDSCRIPT := STM32F103C8TX_FLASH.ld
LIBS     := -lc -lm -lnosys
LDFLAGS  := $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) \
            -Wl,-Map=$(BUILD)/$(TARGET).map,--cref \
            -Wl,--gc-sections

# ── Sources ─────────────────────────────────────────────────────────────────
C_SOURCES := \
  Src/main.c            \
  Src/motor.c           \
  Src/bluetooth.c       \
  Src/stm32f1xx_it.c    \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c             \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c      \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c         \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c       \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c    \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c        \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c     \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c         \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c         \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c      \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c         \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c      \
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c

ASM_SOURCES := \
  Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s

# ── Object files ────────────────────────────────────────────────────────────
OBJECTS  := $(addprefix $(BUILD)/, $(C_SOURCES:.c=.o))
OBJECTS  += $(addprefix $(BUILD)/, $(ASM_SOURCES:.s=.o))

DEPS     := $(OBJECTS:.o=.d)

# ── Default target ──────────────────────────────────────────────────────────
.PHONY: all flash clean

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET).hex
	$(SZ) $(BUILD)/$(TARGET).elf

# ELF
$(BUILD)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# BIN
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(CP) -O binary -S $< $@

# HEX
$(BUILD)/$(TARGET).hex: $(BUILD)/$(TARGET).elf
	$(CP) -O ihex $< $@

# C objects
$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -MMD -MP $< -o $@

# ASM objects
$(BUILD)/%.o: %.s
	@mkdir -p $(@D)
	$(AS) -c $(ASFLAGS) $< -o $@

-include $(DEPS)

# ── Flash via OpenOCD + ST-Link ──────────────────────────────────────────────
flash: $(BUILD)/$(TARGET).bin
	openocd -f interface/stlink.cfg \
	        -f target/stm32f1x.cfg  \
	        -c "program $(BUILD)/$(TARGET).bin verify reset exit 0x08000000"

# ── Clean ────────────────────────────────────────────────────────────────────
clean:
	rm -rf $(BUILD)
