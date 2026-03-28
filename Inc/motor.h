/**
 * @file    motor.h
 * @brief   DC motor control via L298N driver
 *
 * Hardware wiring
 * ---------------
 *  Left motor
 *    IN1 → PB0   (direction)
 *    IN2 → PB1   (direction)
 *    ENA → PA6   (TIM3_CH1 PWM)
 *
 *  Right motor
 *    IN3 → PB10  (direction)
 *    IN4 → PB11  (direction)
 *    ENB → PA7   (TIM3_CH2 PWM)
 *
 * Speed is expressed as a percentage [0–100].
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialise GPIO pins used by the L298N motor driver.
 *         Call once after MX_GPIO_Init() and MX_TIM3_Init().
 */
void Motor_Init(void);

/**
 * @brief  Drive both motors forward at the given speed.
 * @param  speed  Duty-cycle percentage [0–100]
 */
void Motor_Forward(uint8_t speed);

/**
 * @brief  Drive both motors backward at the given speed.
 * @param  speed  Duty-cycle percentage [0–100]
 */
void Motor_Backward(uint8_t speed);

/**
 * @brief  Pivot the car to the left (right motor forward, left motor stopped).
 * @param  speed  Duty-cycle percentage [0–100]
 */
void Motor_TurnLeft(uint8_t speed);

/**
 * @brief  Pivot the car to the right (left motor forward, right motor stopped).
 * @param  speed  Duty-cycle percentage [0–100]
 */
void Motor_TurnRight(uint8_t speed);

/**
 * @brief  Stop both motors immediately.
 */
void Motor_Stop(void);

/**
 * @brief  Set individual motor speed as a duty-cycle percentage.
 * @param  left_speed   Left motor speed  [0–100]
 * @param  right_speed  Right motor speed [0–100]
 */
void Motor_SetSpeed(uint8_t left_speed, uint8_t right_speed);

#endif /* MOTOR_H */
