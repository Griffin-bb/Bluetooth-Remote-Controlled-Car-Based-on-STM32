#ifndef __HC_04BLE_H
#define __HC_04BLE_H

#include <stdio.h>

extern char HC_04BLE_GetDate[];                    //存放数据包位置

void HC_04BLE_Init(void);
void HC_04BLE_SendByte(uint8_t Byte);                               //发送字节
void HC_04BLE_SendArray(uint8_t *Array,uint8_t Length);             //发送数组
void HC_04BLE_SendString(char *String);                             //发送字符串
void HC_04BLE_SendNumber(uint32_t Number, uint8_t Length);          //发送数字
void HC_04BLE_printf(char *format, ...);           //封装printf函数
void USART3_IRQHandler(void);                      //中断接收数据

uint8_t HC_04BLE_Flag(void);                       //读取并清除接收数据包标志位

#endif
