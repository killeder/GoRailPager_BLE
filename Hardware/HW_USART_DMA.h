/********************************************************************************************
文件名：USART_DMA.h - 使用DMA进行数据收发的串口通讯 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年12月17日　　　　  
修改日志：　　
　　NO.1-								
**********************************************************************************************
说明：
**********************************************************************************************/
#ifndef USART_DMA_H
#define USART_DMA_H
void USARTx_DMA_Init(uint32_t BaudRate);	//USARTx及相关DMA初始化函数
uint16_t USARTx_CheckDataArrival(void);		//确认是否收到了数据
void USARTx_StartDMATransmit(uint16_t DatSize);	//开始一次DMA发送
//------接收和发送缓冲区------------------
#define	USARTx_TxBUFFERSIZE		64
#define	USARTx_RxBUFFERSIZE		64
extern uint8_t USARTx_TxBuffer[USARTx_TxBUFFERSIZE];	//发送缓冲区
extern uint8_t USARTx_RxBuffer[USARTx_RxBUFFERSIZE];	//接收缓冲区
#endif
