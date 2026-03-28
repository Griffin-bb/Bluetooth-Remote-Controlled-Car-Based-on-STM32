/**
 * @file    main.h
 * @brief   Bluetooth Remote-Controlled Car — top-level header
 *
 * Target:  STM32F103C8T6 (72 MHz)
 * Toolchain: arm-none-eabi-gcc
 */

#ifndef MAIN_H
#define MAIN_H

#include "stm32f1xx_hal.h"

/* ------------------------------------------------------------------ */
/* Error handler                                                       */
/* ------------------------------------------------------------------ */
void Error_Handler(void);

/* ------------------------------------------------------------------ */
/* Peripheral handles (defined in main.c, shared across modules)       */
/* ------------------------------------------------------------------ */
extern UART_HandleTypeDef  huart1;   /* Bluetooth USART1  (PA9/PA10) */
extern TIM_HandleTypeDef   htim3;    /* PWM timer for motors          */

#endif /* MAIN_H */
