/**
 * @file    stm32f1xx_it.c
 * @brief   STM32F1 interrupt service routines
 *
 * Only the ISRs used by this project are defined here. All other
 * vectors are handled by the default weak stubs in the start-up file.
 */

#include "main.h"

/* ------------------------------------------------------------------ */
/* Cortex-M3 exception handlers                                        */
/* ------------------------------------------------------------------ */

void NMI_Handler(void)
{
    /* Non-maskable interrupt — nothing to do */
}

void HardFault_Handler(void)
{
    /* Spin forever; attach a debugger to inspect the fault frame */
    while (1) { }
}

void MemManage_Handler(void)
{
    while (1) { }
}

void BusFault_Handler(void)
{
    while (1) { }
}

void UsageFault_Handler(void)
{
    while (1) { }
}

void SVC_Handler(void)
{
    /* Reserved for an RTOS if one is added later */
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
    /* Reserved for an RTOS if one is added later */
}

/**
 * @brief  SysTick interrupt handler — drives HAL_GetTick() millisecond counter.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/* ------------------------------------------------------------------ */
/* USART1 interrupt handler                                            */
/* ------------------------------------------------------------------ */

/**
 * @brief  USART1 global interrupt handler.
 *         Delegates to HAL, which will call HAL_UART_RxCpltCallback()
 *         when a complete byte has been received.
 */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}
