/*﻿-----------------------------------------------------------------------
*@file     HW_USART.c
*@brief    USART port utils for debug trace and command parse
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/01/16
-----------------------------------------------------------------------*/
#ifndef HW_USART_H
#define HW_USART_H

void HW_USART1_Init(uint32_t BaudRate);	//USART1 initialize
void HW_USART1_SendByte(uint8_t data);	//USART1 send one byte
int  Trace_printf(const char* format, ...);//trace printf

/*
#define USART1_RXCOMPLETE_FLAG	0x80	//USART1 rx complete flag

extern uint8_t USART1_RxState;			//USART1 rx status
//the MSB of "USART1_RxState" signifies completeness of rx after it was
//set to "1".
extern char USART1_RxBuffer[];		//USART1 rx buffer
*/

#endif
