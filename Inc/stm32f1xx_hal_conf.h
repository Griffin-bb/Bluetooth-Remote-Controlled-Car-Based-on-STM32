/**
 * @file    stm32f1xx_hal_conf.h
 * @brief   HAL configuration — enable only the peripherals used by this project.
 */

#ifndef STM32F1XX_HAL_CONF_H
#define STM32F1XX_HAL_CONF_H

/* ------------------------------------------------------------------ */
/* Tick interrupt priority                                             */
/* ------------------------------------------------------------------ */
#define TICK_INT_PRIORITY   0x0FU

/* ------------------------------------------------------------------ */
/* Module selection                                                    */
/* ------------------------------------------------------------------ */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

/* ------------------------------------------------------------------ */
/* Oscillator values                                                   */
/* ------------------------------------------------------------------ */
#if !defined(HSE_VALUE)
#define HSE_VALUE  8000000U   /* 8 MHz external crystal on Blue-Pill  */
#endif

#if !defined(HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT  100U
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE  8000000U
#endif

#if !defined(LSI_VALUE)
#define LSI_VALUE  40000U
#endif

#if !defined(LSE_VALUE)
#define LSE_VALUE  32768U
#endif

#if !defined(LSE_STARTUP_TIMEOUT)
#define LSE_STARTUP_TIMEOUT  5000U
#endif

/* ------------------------------------------------------------------ */
/* Include HAL module headers                                          */
/* ------------------------------------------------------------------ */
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_uart.h"

/* ------------------------------------------------------------------ */
/* Assertions (disabled in release build)                              */
/* ------------------------------------------------------------------ */
#define assert_param(expr)  ((void)0U)

#endif /* STM32F1XX_HAL_CONF_H */
