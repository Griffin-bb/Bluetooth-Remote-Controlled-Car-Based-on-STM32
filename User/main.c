#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "OLED.h"
#include "Motor.h"
#include "HC_04BLE.h"
#include "Delay.h"

#include <stdlib.h>
#include <string.h>

int8_t Get_Go_Speed ,Get_Turn_Speed;

int8_t Speed_Left(void)
{
	int8_t speed;
    speed = (Get_Go_Speed + Get_Turn_Speed)/2;
}

int8_t Speed_Right(void)
{
	int8_t speed;
    speed = (Get_Go_Speed - Get_Turn_Speed)/2;
}


int main(void)
{
	LED_Init();
	OLED_Init();
	Motor_Init();
	HC_04BLE_Init();
	OLED_ShowString(1,1,"Speed1:");
	OLED_ShowSignedNum(1,8,Speed_Left(),4);
	OLED_ShowString(2,1,"Speed2:");
	OLED_ShowSignedNum(2,8,Speed_Right(),4);

	
	while(1)
	{
		if(HC_04BLE_Flag() == 1)
		{
			OLED_ShowString(3,1,HC_04BLE_GetDate);         //收到指令
			HC_04BLE_printf("%s\r\n",HC_04BLE_GetDate);
			
			strtok(HC_04BLE_GetDate,",");
			Get_Turn_Speed = atoi(strtok(NULL,","));
			Get_Go_Speed = atoi(strtok(NULL,","));
			
			Motor_SetSpeed(MOTOR_Right,Speed_Right());
			Motor_SetSpeed(MOTOR_Left,Speed_Left());
			
			Motor_SetSpeed(MOTOR_Right,0);
			Motor_SetSpeed(MOTOR_Left,0);
		}
		
	}
}
