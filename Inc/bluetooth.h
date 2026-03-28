/**
 * @file    bluetooth.h
 * @brief   Bluetooth (HC-05/HC-06) command interface via USART1
 *
 * Supported single-byte commands
 * --------------------------------
 *  'F' or '1'  → Forward
 *  'B' or '2'  → Backward
 *  'L' or '3'  → Turn Left
 *  'R' or '4'  → Turn Right
 *  'S' or '0'  → Stop
 *  '5'–'9'     → Speed levels 50 % … 90 % (maps digit × 10 %)
 *
 * The module uses non-blocking interrupt-driven reception so the main
 * loop remains responsive.
 */

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Command codes (public so callers can compare against received cmd)  */
/* ------------------------------------------------------------------ */
#define BT_CMD_FORWARD   'F'
#define BT_CMD_BACKWARD  'B'
#define BT_CMD_LEFT      'L'
#define BT_CMD_RIGHT     'R'
#define BT_CMD_STOP      'S'
#define BT_CMD_NONE      0x00

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialise Bluetooth module: start UART interrupt reception.
 *         Must be called after MX_USART1_UART_Init().
 */
void Bluetooth_Init(void);

/**
 * @brief  Return the latest decoded command byte and clear the pending flag.
 *         Returns BT_CMD_NONE (0x00) when no new command is available.
 */
uint8_t Bluetooth_GetCommand(void);

/**
 * @brief  Return the current speed level [0–100] requested over Bluetooth.
 *         Defaults to 75 % until a speed command is received.
 */
uint8_t Bluetooth_GetSpeed(void);

/**
 * @brief  UART receive-complete callback — call from
 *         HAL_UART_RxCpltCallback() in stm32f1xx_it.c.
 */
void Bluetooth_RxCallback(void);

#endif /* BLUETOOTH_H */
