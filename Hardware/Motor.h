#ifndef __MOTOR_H
#define __MOTOR_H

extern uint8_t MOTOR_Right;
extern uint8_t MOTOR_Left;

void Motor_Init(void);
void Motor_PowerState_Turn(void);                               //改变驱动板上电状态
void Motor_SetSpeed(uint8_t Motor_ID, int8_t Speed);             //设置电机速度

#endif
