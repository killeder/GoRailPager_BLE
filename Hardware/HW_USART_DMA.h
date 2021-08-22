/********************************************************************************************
�ļ�����USART_DMA.h - ʹ��DMA���������շ��Ĵ���ͨѶ 
��д�ˣ�лӢ��(E-mail:xieyingnan1994@163.com)���� ��
��дʱ�䣺2018��12��17�ա�������  
�޸���־������
����NO.1-								
**********************************************************************************************
˵����
**********************************************************************************************/
#ifndef USART_DMA_H
#define USART_DMA_H
void USARTx_DMA_Init(uint32_t BaudRate);	//USARTx�����DMA��ʼ������
uint16_t USARTx_CheckDataArrival(void);		//ȷ���Ƿ��յ�������
void USARTx_StartDMATransmit(uint16_t DatSize);	//��ʼһ��DMA����
//------���պͷ��ͻ�����------------------
#define	USARTx_TxBUFFERSIZE		64
#define	USARTx_RxBUFFERSIZE		64
extern uint8_t USARTx_TxBuffer[USARTx_TxBUFFERSIZE];	//���ͻ�����
extern uint8_t USARTx_RxBuffer[USARTx_RxBUFFERSIZE];	//���ջ�����
#endif
