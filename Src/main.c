/**
 * @file    main.c
 * @brief   Bluetooth Remote-Controlled Car — main application
 *
 * System clock: 72 MHz (PLL × 9, HSE 8 MHz)
 *
 * Peripheral summary
 * ------------------
 *  USART1  PA9/PA10   9600 baud  — HC-05/HC-06 Bluetooth module
 *  TIM3    PA6/PA7    1 kHz PWM  — L298N ENA/ENB motor enable pins
 *  GPIOB   PB0/PB1    output     — L298N IN1/IN2 (left motor direction)
 *  GPIOB   PB10/PB11  output     — L298N IN3/IN4 (right motor direction)
 *  GPIOC   PC13       output     — on-board LED (status indicator)
 *
 * Main loop behaviour
 * -------------------
 *  1. Poll Bluetooth_GetCommand() for a fresh command byte.
 *  2. Dispatch to the appropriate Motor_xxx() function.
 *  3. Toggle the status LED when a command is executed.
 */

#include "main.h"
#include "motor.h"
#include "bluetooth.h"

/* ------------------------------------------------------------------ */
/* Peripheral handle definitions (declared extern in main.h)           */
/* ------------------------------------------------------------------ */
UART_HandleTypeDef  huart1;
TIM_HandleTypeDef   htim3;

/* ------------------------------------------------------------------ */
/* Private function prototypes                                         */
/* ------------------------------------------------------------------ */
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);

/* ------------------------------------------------------------------ */
/* Entry point                                                         */
/* ------------------------------------------------------------------ */
int main(void)
{
    /* HAL initialisation and system clock */
    HAL_Init();
    SystemClock_Config();

    /* Peripheral initialisation */
    MX_GPIO_Init();
    MX_TIM3_Init();
    MX_USART1_UART_Init();

    /* Application module initialisation */
    Motor_Init();
    Bluetooth_Init();

    /* Main loop */
    while (1)
    {
        uint8_t cmd   = Bluetooth_GetCommand();
        uint8_t speed = Bluetooth_GetSpeed();

        if (cmd != BT_CMD_NONE)
        {
            switch (cmd)
            {
                case BT_CMD_FORWARD:
                    Motor_Forward(speed);
                    break;

                case BT_CMD_BACKWARD:
                    Motor_Backward(speed);
                    break;

                case BT_CMD_LEFT:
                    Motor_TurnLeft(speed);
                    break;

                case BT_CMD_RIGHT:
                    Motor_TurnRight(speed);
                    break;

                case BT_CMD_STOP:
                    Motor_Stop();
                    break;

                default:
                    break;
            }

            /* Toggle status LED to indicate activity */
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
    }
}

/* ------------------------------------------------------------------ */
/* System clock: 72 MHz via PLL (HSE 8 MHz × 9)                       */
/* ------------------------------------------------------------------ */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef       osc  = {0};
    RCC_ClkInitTypeDef       clk  = {0};

    osc.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState            = RCC_HSE_ON;
    osc.HSEPredivValue      = RCC_HSE_PREDIV_DIV1;
    osc.PLL.PLLState        = RCC_PLL_ON;
    osc.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLMUL          = RCC_PLL_MUL9;   /* 8 MHz × 9 = 72 MHz */
    if (HAL_RCC_OscConfig(&osc) != HAL_OK)
    {
        Error_Handler();
    }

    clk.ClockType           = RCC_CLOCKTYPE_HCLK  | RCC_CLOCKTYPE_SYSCLK |
                              RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource        = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider       = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider      = RCC_HCLK_DIV2;  /* APB1 = 36 MHz (max) */
    clk.APB2CLKDivider      = RCC_HCLK_DIV1;  /* APB2 = 72 MHz       */
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ------------------------------------------------------------------ */
/* GPIO initialisation                                                 */
/* ------------------------------------------------------------------ */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    /* Enable clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Default output level — all low */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 |
                              GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); /* LED off (active-low) */

    /* Motor direction pins: PB0, PB1, PB10, PB11 */
    gpio.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);

    /* Status LED: PC13 (on-board Blue-Pill LED, active-low) */
    gpio.Pin   = GPIO_PIN_13;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio);

    /* PA6, PA7 are configured as TIM3 PWM outputs inside MX_TIM3_Init() */
    /* PA9, PA10 are configured as USART1 AF inside MX_USART1_UART_Init() */
}

/* ------------------------------------------------------------------ */
/* TIM3 — 1 kHz PWM on CH1 (PA6) and CH2 (PA7)                        */
/*   TIM3 is on APB1 (36 MHz); the APB1 prescaler ×2 multiplier gives  */
/*   the timer a 72 MHz input clock.                                   */
/*   PSC = 71  → timer clock = 1 MHz                                   */
/*   ARR = 999 → PWM frequency = 1 kHz                                 */
/* ------------------------------------------------------------------ */
static void MX_TIM3_Init(void)
{
    TIM_OC_InitTypeDef oc = {0};

    __HAL_RCC_TIM3_CLK_ENABLE();

    htim3.Instance               = TIM3;
    htim3.Init.Prescaler         = 71U;
    htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim3.Init.Period            = 999U;   /* ARR */
    htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    oc.OCMode       = TIM_OCMODE_PWM1;
    oc.Pulse        = 0U;   /* start at 0 % duty */
    oc.OCPolarity   = TIM_OCPOLARITY_HIGH;
    oc.OCFastMode   = TIM_OCFAST_DISABLE;

    /* Channel 1 — ENA (left motor) */
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &oc, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Channel 2 — ENB (right motor) */
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &oc, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure PA6, PA7 as alternate-function push-pull */
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin       = GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);
}

/* ------------------------------------------------------------------ */
/* USART1 — 9600 baud, 8N1, interrupt-driven RX                        */
/* ------------------------------------------------------------------ */
static void MX_USART1_UART_Init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();

    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 9600U;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure PA9 (TX) and PA10 (RX) */
    GPIO_InitTypeDef gpio = {0};

    gpio.Pin   = GPIO_PIN_9;          /* TX */
    gpio.Mode  = GPIO_MODE_AF_PP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin   = GPIO_PIN_10;         /* RX */
    gpio.Mode  = GPIO_MODE_INPUT;
    gpio.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* Enable USART1 interrupt in NVIC */
    HAL_NVIC_SetPriority(USART1_IRQn, 0U, 0U);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/* ------------------------------------------------------------------ */
/* HAL callback — UART receive complete                                */
/* ------------------------------------------------------------------ */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        Bluetooth_RxCallback();
    }
}

/* ------------------------------------------------------------------ */
/* Error handler                                                       */
/* ------------------------------------------------------------------ */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* Trap on unrecoverable error — toggle LED rapidly for debugging */
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        for (volatile uint32_t i = 0; i < 500000UL; i++) { }
    }
}
