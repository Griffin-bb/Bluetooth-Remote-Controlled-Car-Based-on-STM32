/**
 * @file    bluetooth.c
 * @brief   Bluetooth command reception via USART1 (interrupt-driven)
 *
 * The HC-05/HC-06 module is connected to:
 *   USART1 TX → PA9
 *   USART1 RX → PA10
 *   Baud rate : 9600 (default for HC-06) or 38400 (common HC-05 default)
 *
 * Each received byte is interpreted as a single command character.
 */

#include "bluetooth.h"
#include "main.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* Private state                                                       */
/* ------------------------------------------------------------------ */

static volatile uint8_t rx_byte    = 0;      /* raw byte from ISR            */
static volatile uint8_t cmd_ready  = 0;      /* set when a new cmd is valid  */
static volatile uint8_t cmd_byte   = BT_CMD_NONE;
static volatile uint8_t cur_speed  = 75U;    /* default speed 75 %           */

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

void Bluetooth_Init(void)
{
    /* Arm first 1-byte interrupt reception */
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_byte, 1U);
}

uint8_t Bluetooth_GetCommand(void)
{
    if (!cmd_ready)
    {
        return BT_CMD_NONE;
    }

    cmd_ready = 0;
    return (uint8_t)cmd_byte;
}

uint8_t Bluetooth_GetSpeed(void)
{
    return cur_speed;
}

void Bluetooth_RxCallback(void)
{
    uint8_t byte = (uint8_t)rx_byte;

    /* Decode speed digits first ('0'–'9' → 0 %–90 %) */
    if (byte >= '0' && byte <= '9')
    {
        uint8_t level = (uint8_t)(byte - '0');

        if (level == 0U)
        {
            /* '0' → stop command */
            cmd_byte  = BT_CMD_STOP;
            cmd_ready = 1;
        }
        else
        {
            /* '1'–'9' → map to speed 10 %–90 % */
            cur_speed = (uint8_t)(level * 10U);

            /* Also treat '1'–'4' as directional shortcuts */
            switch (byte)
            {
                case '1':  cmd_byte = BT_CMD_FORWARD;   cmd_ready = 1; break;
                case '2':  cmd_byte = BT_CMD_BACKWARD;  cmd_ready = 1; break;
                case '3':  cmd_byte = BT_CMD_LEFT;       cmd_ready = 1; break;
                case '4':  cmd_byte = BT_CMD_RIGHT;      cmd_ready = 1; break;
                default:   /* '5'–'9' are pure speed updates */           break;
            }
        }
    }
    else
    {
        /* Letter commands — case-insensitive */
        uint8_t upper = (byte >= 'a' && byte <= 'z') ? (uint8_t)(byte - 32U) : byte;

        switch (upper)
        {
            case BT_CMD_FORWARD:
            case BT_CMD_BACKWARD:
            case BT_CMD_LEFT:
            case BT_CMD_RIGHT:
            case BT_CMD_STOP:
                cmd_byte  = upper;
                cmd_ready = 1;
                break;
            default:
                /* Unknown byte — ignore */
                break;
        }
    }

    /* Re-arm interrupt for the next byte */
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_byte, 1U);
}
