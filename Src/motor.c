/**
 * @file    motor.c
 * @brief   DC motor control via L298N dual H-bridge driver
 *
 * PWM channels
 * ------------
 *  TIM3_CH1 (PA6)  → ENA (left motor speed)
 *  TIM3_CH2 (PA7)  → ENB (right motor speed)
 *
 * Direction pins (active-high, both low = brake/coast)
 * ----------------------------------------------------
 *  PB0  → IN1 (left  motor, forward phase)
 *  PB1  → IN2 (left  motor, reverse phase)
 *  PB10 → IN3 (right motor, forward phase)
 *  PB11 → IN4 (right motor, reverse phase)
 */

#include "motor.h"
#include "main.h"

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/* TIM3 auto-reload value configured in MX_TIM3_Init() to yield 1 kHz PWM */
#define MOTOR_TIM_ARR  999U   /* ARR = (72 MHz / (PSC+1) / freq) − 1       */

/**
 * @brief  Set PWM duty cycle for TIM3 channel 1 (left motor).
 * @param  duty  Percentage [0–100]
 */
static void pwm_set_left(uint8_t duty)
{
    if (duty > 100U) { duty = 100U; }
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,
                          (uint32_t)duty * (MOTOR_TIM_ARR + 1U) / 100U);
}

/**
 * @brief  Set PWM duty cycle for TIM3 channel 2 (right motor).
 * @param  duty  Percentage [0–100]
 */
static void pwm_set_right(uint8_t duty)
{
    if (duty > 100U) { duty = 100U; }
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2,
                          (uint32_t)duty * (MOTOR_TIM_ARR + 1U) / 100U);
}

/* ------------------------------------------------------------------ */
/* Direction pin helpers                                               */
/* ------------------------------------------------------------------ */

static void left_forward(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

static void left_backward(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
}

static void left_stop(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET);
}

static void right_forward(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
}

static void right_backward(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
}

static void right_stop(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

void Motor_Init(void)
{
    /* Start PWM output on both channels */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    /* Ensure motors are stopped at startup */
    Motor_Stop();
}

void Motor_Forward(uint8_t speed)
{
    left_forward();
    right_forward();
    pwm_set_left(speed);
    pwm_set_right(speed);
}

void Motor_Backward(uint8_t speed)
{
    left_backward();
    right_backward();
    pwm_set_left(speed);
    pwm_set_right(speed);
}

void Motor_TurnLeft(uint8_t speed)
{
    left_stop();
    right_forward();
    pwm_set_left(0U);
    pwm_set_right(speed);
}

void Motor_TurnRight(uint8_t speed)
{
    left_forward();
    right_stop();
    pwm_set_left(speed);
    pwm_set_right(0U);
}

void Motor_Stop(void)
{
    left_stop();
    right_stop();
    pwm_set_left(0U);
    pwm_set_right(0U);
}

void Motor_SetSpeed(uint8_t left_speed, uint8_t right_speed)
{
    pwm_set_left(left_speed);
    pwm_set_right(right_speed);
}
