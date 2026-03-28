#include "stm32f10x.h"                  // Device header
#include "LED.h"

#include <stdio.h>
#include <stdarg.h>

char HC_04BLE_GetDate[100];                       //存放数据包位置
uint8_t HC_04BLE_GetDate_Flag;                   //接收数据包标志位，接收数据包后置 1

void HC_04BLE_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;                           //USART3_Tx接HC_04_Rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                  //USART3_Rx接HC_04_Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStructure);
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);                   //接收中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART3,ENABLE);
}

void HC_04BLE_SendByte(uint8_t Byte)                               //发送字节
{
	USART_SendData(USART3,Byte);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
}

void HC_04BLE_SendArray(uint8_t *Array,uint8_t Length)             //发送数组
{
	uint8_t i;
	for(i = 0;i < Length;i ++)
	{
		HC_04BLE_SendByte(Array[i]);
	}
}

void HC_04BLE_SendString(char *String)                             //发送字符串
{
	uint8_t i;
	for(i = 0;String[i] != '\0';i ++)
	{
	    HC_04BLE_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)                        //计算平方
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void HC_04BLE_SendNumber(uint32_t Number, uint8_t Length)          //发送数字
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		HC_04BLE_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
		/*依次发送数字的每一位*/
	}
}

int fputc(int ch, FILE *f)			             //将printf的底层重定向到自己的发送字节函数
{
	HC_04BLE_SendByte(ch);
	return ch;
}


void HC_04BLE_printf(char *format, ...)          //封装printf函数
{
	char String[100];				   
	va_list arg;					   
	va_start(arg, format);			   
	vsprintf(String, format, arg);	   
	va_end(arg);					   
	HC_04BLE_SendString(String);       
}

void USART3_IRQHandler(void)                    //中断接收数据
{
    static uint8_t RxState;                          //接收状态机状态,进入接收状态置 1
	static uint8_t pRxPlace;                         //接收状态机接收位置
	                                             
	uint8_t RxDate;                                  //串口数据暂存位置
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
	{
		RxDate = USART_ReceiveData(USART3);          //接收数据触发中断，将数据存放至RxDate
		
		if(RxState == 0)
		{
			if(RxDate == '[')                        //接收到'['进入接收状态，RxState置 1
			{
				LED_ON();
			    RxState = 1;
				pRxPlace = 0;
			}
		}
		else
		{
		    if(RxDate != ']')                        //依次存放数据到数据包
			{
				HC_04BLE_GetDate[pRxPlace] = RxDate;
				pRxPlace ++;
			}
			else
			{
				RxState = 0;                         //接收到']'结束接收状态，RxState置 0
				HC_04BLE_GetDate[pRxPlace] = '\0';
				HC_04BLE_GetDate_Flag = 1;           //接收数据包标志位置 1，通知主函数处理
				LED_OFF();
			}
		}
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
}

uint8_t HC_04BLE_Flag(void)                          //读取并清除接收数据包标志位
{
	if(HC_04BLE_GetDate_Flag == 1)
	{
	    HC_04BLE_GetDate_Flag = 0;
		return 1;
	}
	else
	{
	    return 0;
	}
}
