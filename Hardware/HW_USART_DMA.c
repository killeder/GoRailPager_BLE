/*********************************************************************************************
文件名：USART_DMA.h - 使用DMA进行数据收发的串口通讯　 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年12月18日　　　　  
修改日志：								
**********************************************************************************************
说明：
**********************************************************************************************/
#include "stm32f10x.h"
#include "Usart_DMA.h"
//---------------GPIO定义----------------
#define	USARTx_GPIO_PORT	GPIOA
#define USARTx_GPIO_CLOCK	RCC_APB2Periph_GPIOA
#define USARTx_RxPIN		GPIO_Pin_10
#define	USARTx_TxPIN		GPIO_Pin_9
//--------------使用的USART定义-----------
#define USARTx 				USART1
#define USARTx_CLOCK 		RCC_APB2Periph_USART1
//--------------中断线定义----------------
#define	USARTx_IRQ			USART1_IRQn 		//串口中断线
#define	USARTx_IRQHandler	USART1_IRQHandler	//串口中断响应函数
#define	Tx_DMA_CH_IRQ		DMA1_Channel4_IRQn 	//DMA串口发送通道的中断线
#define	Tx_DMA_CH_IRQHandler DMA1_Channel4_IRQHandler//DMA串口发送通道的中断响应函数
#define	Tx_DMA_CH_IT_TC		DMA1_IT_TC4	//DMA串口发送通道传输完毕中断标志位
//--------------DMA通道定义--------------
#define	USARTx_Rx_DMA_CH	DMA1_Channel5	
#define USARTx_Tx_DMA_CH	DMA1_Channel4
//------串口状态标识----------------------
#define	USART_STATE_RECEIVED	0x01	//接收数据完成状态标识
#define	USART_STATE_NORMAL		0x00	//正常状态标识
static uint8_t USARTx_Stat  = USART_STATE_NORMAL;//模块私有的串口状态标识，初始化为NORMAL
static uint16_t ReceivedDataSize;	//模块私有变量，收到数据的长度
//------接收和发送缓冲区------------------
uint8_t USARTx_TxBuffer[USARTx_TxBUFFERSIZE];	//发送缓冲区
uint8_t USARTx_RxBuffer[USARTx_RxBUFFERSIZE];	//接收缓冲区

void USARTx_IRQHandler(void);	//串口USARTx中断响应函数
void Tx_DMA_CH_IRQHandler(void);	//DMA串口发送通道中断相应函数
/*---------------------------------------------------------------------------
函数名：USARTx_DMA_Init
功能：USARTx+中断+对应DMA发送和接收通道初始化函数
输入参数：uint32_t BaudRate - 通信波特率
返回值：无
---------------------------------------------------------------------------*/ 	   
void USARTx_DMA_Init(uint32_t BaudRate)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	DMA_InitTypeDef		DMA_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
//-----------------------1.GPIO配置--------------------------------------
	RCC_APB2PeriphClockCmd(USARTx_GPIO_CLOCK, ENABLE);
	//USARTx_TX初始化
	GPIO_InitStructure.GPIO_Pin = USARTx_TxPIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//Tx复用推挽输出
	GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);  
	//USARTx_RX初始化
	GPIO_InitStructure.GPIO_Pin = USARTx_RxPIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//Rx浮空输入
	GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);
//------------------2.串行口和DMA串口发送通道的中断线配置------------------
	//2.1 USARTx的中断线配置
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
	//2.2 DMA串口发送通道的中断线配置
	NVIC_InitStructure.NVIC_IRQChannel = Tx_DMA_CH_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
//------------------3.DMA串口发送通道和接收通道配置------------------------
	//3.1 打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	//打开DMA1时钟
	//3.2初始化DMA对应USARTx接收的通道
	DMA_DeInit(USARTx_Rx_DMA_CH);	//复位DMA通道配置
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)(&(USARTx->DR));//外设基地址：USARTx数据寄存器地址
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)USARTx_RxBuffer;	//内存基地址：接收数据缓冲区地址
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;	//数据来源于外设
	DMA_InitStruct.DMA_BufferSize = USARTx_RxBUFFERSIZE;	//缓冲区大小：数据目的地大小
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设的地址不递增
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存的地址递增
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据大小为8位
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//内存数据大小也为8位
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;	//正常模式，收发完一次就不再收发了而不是循环收发
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;	//优先级：高
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;	//禁用存储器到存储器模式，因为这里是外设到存储器模式
	DMA_Init(USARTx_Rx_DMA_CH,&DMA_InitStruct);	//初始化DMA通道
	DMA_Cmd(USARTx_Rx_DMA_CH,ENABLE);	//使能串口接收DMA通道	
	//3.3初始化DMA对应USARTx发送的通道
	DMA_DeInit(USARTx_Tx_DMA_CH);	//复位DMA通道配置
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)USARTx_TxBuffer;	//内存基地址：发送数据缓冲区地址
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;	//数据送往外设
	DMA_InitStruct.DMA_BufferSize = USARTx_TxBUFFERSIZE;	//缓冲区大小：数据目的地大小
	DMA_Init(USARTx_Tx_DMA_CH,&DMA_InitStruct);	//初始化DMA通道
	DMA_ITConfig(USARTx_Tx_DMA_CH,DMA_IT_TC,ENABLE);	//将传输完成标志位与中断绑定
	DMA_Cmd(USARTx_Tx_DMA_CH,DISABLE);	//先关闭串口发送DMA通道
//-----------------4.初始化USARTx-----------------------------------------
	RCC_APB2PeriphClockCmd(USARTx_CLOCK, ENABLE);	//打开USARTx时钟
	USART_InitStruct.USART_BaudRate = BaudRate;//串口波特率
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStruct.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USARTx,&USART_InitStruct); //初始化串口
	USART_ITConfig(USARTx,USART_IT_IDLE,ENABLE);//将接收完成总线空闲标志位与中断绑定
	USART_DMACmd(USARTx,USART_DMAReq_Rx,ENABLE);	//使能接收DMA请求
	USART_DMACmd(USARTx,USART_DMAReq_Tx,ENABLE);	//使能发送DMA请求
	USART_Cmd(USARTx, ENABLE);	//使能串口	
}
/*---------------------------------------------------------------------------
函数名：USARTx_IRQHandler
功能：串口USARTx中断响应函数
输入参数：无
返回值：无
---------------------------------------------------------------------------*/
void USARTx_IRQHandler(void)
{
	if(USART_GetITStatus(USARTx,USART_IT_IDLE) == SET)
	{
		USART_ClearITPendingBit(USARTx,USART_IT_IDLE);
		USART_ReceiveData(USARTx);
		DMA_Cmd(USARTx_Rx_DMA_CH,DISABLE);	//先关闭接收DMA避免干扰
		ReceivedDataSize = USARTx_RxBUFFERSIZE - 
							 DMA_GetCurrDataCounter(USARTx_Rx_DMA_CH);
							 //获取已收到数据的大小
		DMA_SetCurrDataCounter(USARTx_Rx_DMA_CH,USARTx_RxBUFFERSIZE);
		DMA_Cmd(USARTx_Rx_DMA_CH,ENABLE);	//重启DMA接收通道
		USARTx_Stat |= USART_STATE_RECEIVED;	//设置接收完成标志位
	}
}
/*---------------------------------------------------------------------------
函数名：USARTx_CheckDataArrival
功能：检查是否收到了数据
输入参数：无
返回值：uint16_t - 0：没收到数据；other:收到数据，返回收到数据的长度
---------------------------------------------------------------------------*/
uint16_t USARTx_CheckDataArrival(void)
{
	if(USARTx_Stat & USART_STATE_RECEIVED)
	{
		USARTx_Stat &= ~USART_STATE_RECEIVED;	//清除接收成功标志位
		return ReceivedDataSize;
	}
	else
		return 0;	//没收到数据 返回0
}
/*---------------------------------------------------------------------------
函数名：Tx_DMA_CH_IRQHandler
功能：DMA串口发送通道中断响应函数
输入参数：无
返回值：无
---------------------------------------------------------------------------*/
void Tx_DMA_CH_IRQHandler(void)
{
	if(DMA_GetITStatus(Tx_DMA_CH_IT_TC) == SET)	//如果发送通道传送完毕
	{
		DMA_ClearITPendingBit(Tx_DMA_CH_IT_TC);
		DMA_Cmd(USARTx_Tx_DMA_CH,DISABLE);	//关闭发送DMA通道
	}
}
/*---------------------------------------------------------------------------
函数名：USARTx_StartDMATransmit
功能：开始一次DMA发送
输入参数：uint16_t DatSize - 要传送的数据长度
返回值：无
---------------------------------------------------------------------------*/
void USARTx_StartDMATransmit(uint16_t DatSize)
{
	if(DatSize > USARTx_TxBUFFERSIZE)
		DatSize = USARTx_TxBUFFERSIZE;	//限制DatSize不超过发送缓冲区大小
	DMA_SetCurrDataCounter(USARTx_Tx_DMA_CH,DatSize);//设置DMA传送数据量
	DMA_Cmd(USARTx_Tx_DMA_CH,ENABLE);//启动DMA发送
}
