/**
  ******************************************************************************
  * @file    HW_USART_DMA.c
  * @author  Xie Yingnan(xieyingnan1994@163.com)
  * @version V1.0.0
  * @date    2021-08-23
  * @brief   This file provides functions to transfer data through USART, based
  * 		 on DMA(direct memory access).
  ******************************************************************************
**/
#include "Hardware.h"

#define USART_DMA_TXSIZE	128	//trace printf buffer size
#define USART_DMA_RXSIZE	128	//USART1 Rx buffer size
uint8_t USART_DMA_TxBuffer[USART_DMA_TXSIZE];//buffer for DMA send
char USART_DMA_RxBuffer[USART_DMA_RXSIZE];//buffer for DMA receiving
volatile bool bUSART_DMA_RxFinish = false;//Flag indicating USART DMA Rx finished
volatile uint16_t USART_DMA_RxLength = 0; //USART DMA received data length

/**
  * @brief  inner module for init USART with related GPIOs
  * @param  baudrate: USART baudrate
  * @note   The USART init target is USART2 while GPIO is PA2 and PA3
  * @retval None
  */
static void _USART_GPIO_Init(uint32_t BaudRate)
{
    //GPIO port setting
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);//open PORTA clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//open USART2 clock

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);  //PA2 for USART2 Tx
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);	//PA3 for USART2 Rx
    //USART init.
    USART_InitStructure.USART_BaudRate = BaudRate;//baud rate
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8-bit data
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//1-bit stop
    USART_InitStructure.USART_Parity = USART_Parity_No;//none parity check
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//no flow control
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure); //init. usart2
    //USART2 NVIC setting

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1 ;//priority(0-3)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//enableIRQ channel
    NVIC_Init(&NVIC_InitStructure);	//initialize nvic

    USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//toggle on USART2 IDLE interrupt
    USART_DMACmd(USART2,USART_DMAReq_Tx|USART_DMAReq_Rx,ENABLE);//enable USART DMA Tx&Rx request
    USART_Cmd(USART2,ENABLE);//enable usart2
}
/**
  * @brief  Initializes the USART peripheral according to the baudrate as well as 
  * 		related GPIOs and DMA 
  * @param  baudrate: serialport baudrate
  * @note   The init target here is USART2
  * @retval None
  */
void HW_USART_DMA_Init(uint32_t baudrate)
{
  DMA_InitTypeDef DMA_InitStructure;

  //init USART with its related GPIOs, as well as enable DMA request  
  _USART_GPIO_Init(baudrate);
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//enable clock of DMA controller
  //init DMA1-Channel5 (triggered by USART2 Rx DMA request)
  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->RDR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART_DMA_RxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = USART_DMA_RXSIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5,&DMA_InitStructure);
  DMA_Cmd(DMA1_Channel5,ENABLE);  //enable rx DMA channel
  //init DMA1-Channel4 (triggered by USART2 Tx DMA request)
  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->TDR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART_DMA_TxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//make priority higher than rx dma channel
  DMA_Init(DMA1_Channel4,&DMA_InitStructure);
  //notice:do not enalbe Tx DMA channel here, enable it only before data
  //sending, and disable it after data sending completed
}
/**
  * @brief  Send buffered data bulk to USART via DMA 
  * @param  buff: base address of data buffer
  * @param	length: data length of buffer 
  * @retval None
  */
void HW_USART_DMA_SendBuffer(uint8_t *buff,uint16_t length)
{
  if(length == 0)
    return;
  if(length > USART_DMA_TXSIZE)
    length = USART_DMA_TXSIZE;
  memcpy(USART_DMA_TxBuffer,buff,length);
  //utilize Tx DMA to send data via USART
  DMA_Cmd(DMA1_Channel4,DISABLE);  //first turn off Tx DMA channel
  DMA_SetCurrDataCounter(DMA1_Channel4,length);//then set data length to send
  DMA_Cmd(DMA1_Channel4,ENABLE);  //finally turn on DMA channel to send data
}
/**
  * @brief  clear rx data buffer and flags 
  * @retval None
  */
void HW_USART_DMA_RxClear(void)
{
  //clear rx length, flag of rx finisned, and rx buffer
  USART_DMA_RxLength = 0;
  bUSART_DMA_RxFinish = false;
  memset(USART_DMA_RxBuffer,0,USART_DMA_RXSIZE);
  //re-enable Rx DMA channel and reset data counter
  DMA_Cmd(DMA1_Channel5,DISABLE);
  DMA_SetCurrDataCounter(DMA1_Channel5,USART_DMA_RXSIZE);
  DMA_Cmd(DMA1_Channel5,ENABLE);
}
/**
  * @brief  USART2 IRQ handler 
  * @retval None
  */
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  //if IDLE interruption arrived
	{
    if(bUSART_DMA_RxFinish == false)
    {
      DMA_Cmd(DMA1_Channel5,DISABLE);   
      USART_DMA_RxLength = USART_DMA_RXSIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
      bUSART_DMA_RxFinish = true;
      //re-enable Rx DMA channel for receiving next dataframe
      DMA_SetCurrDataCounter(DMA1_Channel5,USART_DMA_RXSIZE);
      DMA_Cmd(DMA1_Channel5,ENABLE);
    }
    USART_ClearITPendingBit(USART2,USART_IT_IDLE);
  }
}