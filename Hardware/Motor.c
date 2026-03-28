#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "Delay.h"

#define AIN1_PIN          GPIO_Pin_1
#define AIN2_PIN          GPIO_Pin_2
#define AIN_PORT          GPIOA

#define BIN1_PIN          GPIO_Pin_4
#define BIN2_PIN          GPIO_Pin_5
#define BIN_PORT          GPIOA

#define PWMA_PIN          GPIO_Pin_0
#define PWMB_PIN          GPIO_Pin_3

#define STBY_PIN          GPIO_Pin_11
#define STBY_PORT         GPIOA

uint8_t MOTOR_Right = 1;
uint8_t MOTOR_Left = 2;

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	/*初始化控制引脚*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = AIN1_PIN | AIN2_PIN | BIN1_PIN | BIN2_PIN| STBY_PIN ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*用PA0，PA3输出PWMA，PWMB*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = PWMA_PIN | PWMB_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*初始化时基单元*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	/*初始化输出比较*/
	TIM_OCInitTypeDef TIM_OCInitStructure;						
	TIM_OCStructInit(&TIM_OCInitStructure);
	                                                             
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;							
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	TIM_Cmd(TIM2,ENABLE);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_11);                           //初始化后默认驱动板上电
}

void Motor_PowerState_Turn(void)                               //改变驱动板上电状态
{
	if(GPIO_ReadOutputDataBit(STBY_PORT,STBY_PIN) == 0)
	{
	    GPIO_SetBits(STBY_PORT,STBY_PIN);
	}
	else
	{
	    GPIO_ResetBits(STBY_PORT,STBY_PIN);
	}
}

void Motor_SetSpeed(uint8_t Motor_ID, int8_t Speed)               //设置电机速度
{
	if(Motor_ID == 1)                                 //右
	{
		if(Speed >= 0)
		{
			GPIO_ResetBits(AIN_PORT,AIN1_PIN);
			GPIO_SetBits(AIN_PORT,AIN2_PIN);
	        TIM_SetCompare1(TIM2,Speed);
		}
		else
		{
			GPIO_ResetBits(AIN_PORT,AIN2_PIN);
			GPIO_SetBits(AIN_PORT,AIN1_PIN);
	        TIM_SetCompare1(TIM2,-Speed);
		}
	}
	else if(Motor_ID == 2)                             //左
	{
		if(Speed >= 10)
		{
			GPIO_ResetBits(BIN_PORT,BIN1_PIN);
			GPIO_SetBits(BIN_PORT,BIN2_PIN);
	        TIM_SetCompare4(TIM2,Speed);
		}
		else
		{
			GPIO_ResetBits(BIN_PORT,BIN2_PIN);
			GPIO_SetBits(BIN_PORT,BIN1_PIN);
	        TIM_SetCompare4(TIM2,-Speed);
		}
	}
}
