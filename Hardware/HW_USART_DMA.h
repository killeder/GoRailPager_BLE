/**
  ******************************************************************************
  * @file    HW_USART_DMA.h
  * @author  Xie Yingnan(xieyingnan1994@163.com)
  * @version V1.0.0
  * @date    2021-08-23
  * @brief   This file provides functions to transfer data through USART, based
  * 		 on DMA(direct memory access).
  ******************************************************************************
**/
#ifndef HW_USART_DMA_H
#define HW_USART_DMA_H

void HW_USART_DMA_Init(uint32_t baudrate);  //USART and DMA initialization
void HW_USART_DMA_SendBuffer(uint8_t *buff,uint16_t length);	//send buffer
void HW_USART_DMA_RxClear(void);   //clear usart rx buffer and flags

extern volatile uint16_t USART_DMA_RxLength;	//USART1 received data length
extern volatile bool bUSART_DMA_RxFinish;   //Flag indicating USART1 Rx finished
extern char USART_DMA_RxBuffer[];		//USART DMA rx buffer

#endif