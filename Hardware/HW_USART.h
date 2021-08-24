/*-----------------------------------------------------------------------
*@file     HW_USART.c
*@brief    USART port utils for debug trace and command parse
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/08/23
-----------------------------------------------------------------------*/
#ifndef HW_USART_H
#define HW_USART_H

void HW_USART1_Init(uint32_t BaudRate);	//USART1 initialize
void HW_USART1_SendByte(uint8_t data);	//USART1 send one byte
void HW_USART1_RxClear(void);   //clear usart rx buffer and flags
int  Trace_printf(const char* format, ...);//trace printf

extern volatile uint16_t USART1_RxLength;	//USART1 received data length
extern volatile bool bUSART1_RxFinish;   //Flag indicating USART1 Rx finished
extern char USART1_RxBuffer[];		//USART1 rx buffer

#endif
