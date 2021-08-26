/*-----------------------------------------------------------------------
*@file     HW_USART.c
*@brief    USART port utils for debug trace and command parse
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/01/16
-----------------------------------------------------------------------*/
#include "Hardware.h"

#define USART1_TX_BUFFER_SIZE	128	//trace printf buffer size
#define USART1_RX_BUFFER_SIZE	128	//USART1 Rx buffer size
char USART1_TxBuffer[USART1_TX_BUFFER_SIZE];//buffer for trace printf
char USART1_RxBuffer[USART1_RX_BUFFER_SIZE];   //rx buffer, suffixed by '\0'
volatile bool bUSART1_RxFinish = false;   //Flag indicating USART1 Rx finished
volatile uint16_t USART1_RxLength = 0;     //USART1 received data length
/*-----------------------------------------------------------------------
*@brief		trace printf
*@param		formatted string like std printf() function
*@retval	int
-----------------------------------------------------------------------*/
int Trace_printf(const char* format, ...)
{
  int ret;
  va_list ap;

  va_start (ap, format);

  // Print to the local buffer
  ret = vsnprintf (USART1_TxBuffer, sizeof(USART1_TxBuffer), format, ap);
  if (ret > 0)
  {						// Transfer the buffer to the device
  	for(int i = 0; i < ret; i++)
  	{
  		HW_USART1_SendByte((uint8_t)USART1_TxBuffer[i]);
  	}
  }

  va_end (ap);
  return ret;
}
/*-----------------------------------------------------------------------
*@brief		USART1 initialization
*@param		baudrate
*@retval	none
-----------------------------------------------------------------------*/
void HW_USART1_Init(uint32_t BaudRate)
{
	//GPIO port setting
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);//open port clock
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//open usart clock

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);	
    //USART init.
	USART_InitStructure.USART_BaudRate = BaudRate;//baud rate
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8-bit data
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1-bit stop
	USART_InitStructure.USART_Parity = USART_Parity_No;//none parity check
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//no flow control
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); //init. usart1
	//USART1 NVIC setting

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1 ;//priority（0-3）
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//enableIRQ channel
	NVIC_Init(&NVIC_InitStructure);	//initialize nvic
  
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//toggle on rx interrupt
	
	USART_Cmd(USART1,ENABLE);//enable usart1
}
/*-----------------------------------------------------------------------
*@brief		USART1 rx interruption serving routain
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
	char Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //if rx interruption arrived
	{
		Res = (char)USART_ReceiveData(USART1);	//read one byte
		if(bUSART1_RxFinish == false)//if receiving not completed
		{
			if(Res == '\r' || Res == '\n')//if meets <CR> or <LF> character
			{
				USART1_RxBuffer[USART1_RxLength] = '\0';
				//fill current position in rx buffer with '\0'
				bUSART1_RxFinish = true;//setup rx finish flag
			}
			else
			{
				USART1_RxBuffer[USART1_RxLength++] = Res;
				//fill in the rx buffer and move rx pointer forward
				if(USART1_RxLength >= USART1_RX_BUFFER_SIZE)	//if pointer out-of-range
					//aka. "Res" occupied the position to be held by the suffix '\0'
					USART1_RxLength = 0;//clear buffer and restart receiving
			}
		}
    }	
}
/*-----------------------------------------------------------------------
*@brief		send one byte via USART1
*@param		uint8_t data - byte data to send
*@retval	none
-----------------------------------------------------------------------*/
void HW_USART1_SendByte(uint8_t data)
{
	USART_SendData(USART1,data);
	while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}
/*-----------------------------------------------------------------------
*@brief		clear usart rx buffer and flags
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void HW_USART1_RxClear(void)
{
	bUSART1_RxFinish = false;
	USART1_RxLength = 0;
	memset(USART1_RxBuffer,0,USART1_RX_BUFFER_SIZE);
}
