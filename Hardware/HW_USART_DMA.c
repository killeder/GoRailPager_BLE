/*********************************************************************************************
�ļ�����USART_DMA.h - ʹ��DMA���������շ��Ĵ���ͨѶ�� 
��д�ˣ�лӢ��(E-mail:xieyingnan1994@163.com)���� ��
��дʱ�䣺2018��12��18�ա�������  
�޸���־��								
**********************************************************************************************
˵����
**********************************************************************************************/
#include "stm32f10x.h"
#include "Usart_DMA.h"
//---------------GPIO����----------------
#define	USARTx_GPIO_PORT	GPIOA
#define USARTx_GPIO_CLOCK	RCC_APB2Periph_GPIOA
#define USARTx_RxPIN		GPIO_Pin_10
#define	USARTx_TxPIN		GPIO_Pin_9
//--------------ʹ�õ�USART����-----------
#define USARTx 				USART1
#define USARTx_CLOCK 		RCC_APB2Periph_USART1
//--------------�ж��߶���----------------
#define	USARTx_IRQ			USART1_IRQn 		//�����ж���
#define	USARTx_IRQHandler	USART1_IRQHandler	//�����ж���Ӧ����
#define	Tx_DMA_CH_IRQ		DMA1_Channel4_IRQn 	//DMA���ڷ���ͨ�����ж���
#define	Tx_DMA_CH_IRQHandler DMA1_Channel4_IRQHandler//DMA���ڷ���ͨ�����ж���Ӧ����
#define	Tx_DMA_CH_IT_TC		DMA1_IT_TC4	//DMA���ڷ���ͨ����������жϱ�־λ
//--------------DMAͨ������--------------
#define	USARTx_Rx_DMA_CH	DMA1_Channel5	
#define USARTx_Tx_DMA_CH	DMA1_Channel4
//------����״̬��ʶ----------------------
#define	USART_STATE_RECEIVED	0x01	//�����������״̬��ʶ
#define	USART_STATE_NORMAL		0x00	//����״̬��ʶ
static uint8_t USARTx_Stat  = USART_STATE_NORMAL;//ģ��˽�еĴ���״̬��ʶ����ʼ��ΪNORMAL
static uint16_t ReceivedDataSize;	//ģ��˽�б������յ����ݵĳ���
//------���պͷ��ͻ�����------------------
uint8_t USARTx_TxBuffer[USARTx_TxBUFFERSIZE];	//���ͻ�����
uint8_t USARTx_RxBuffer[USARTx_RxBUFFERSIZE];	//���ջ�����

void USARTx_IRQHandler(void);	//����USARTx�ж���Ӧ����
void Tx_DMA_CH_IRQHandler(void);	//DMA���ڷ���ͨ���ж���Ӧ����
/*---------------------------------------------------------------------------
��������USARTx_DMA_Init
���ܣ�USARTx+�ж�+��ӦDMA���ͺͽ���ͨ����ʼ������
���������uint32_t BaudRate - ͨ�Ų�����
����ֵ����
---------------------------------------------------------------------------*/ 	   
void USARTx_DMA_Init(uint32_t BaudRate)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	DMA_InitTypeDef		DMA_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
//-----------------------1.GPIO����--------------------------------------
	RCC_APB2PeriphClockCmd(USARTx_GPIO_CLOCK, ENABLE);
	//USARTx_TX��ʼ��
	GPIO_InitStructure.GPIO_Pin = USARTx_TxPIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//Tx�����������
	GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);  
	//USARTx_RX��ʼ��
	GPIO_InitStructure.GPIO_Pin = USARTx_RxPIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//Rx��������
	GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);
//------------------2.���пں�DMA���ڷ���ͨ�����ж�������------------------
	//2.1 USARTx���ж�������
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	//2.2 DMA���ڷ���ͨ�����ж�������
	NVIC_InitStructure.NVIC_IRQChannel = Tx_DMA_CH_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
//------------------3.DMA���ڷ���ͨ���ͽ���ͨ������------------------------
	//3.1 ��DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	//��DMA1ʱ��
	//3.2��ʼ��DMA��ӦUSARTx���յ�ͨ��
	DMA_DeInit(USARTx_Rx_DMA_CH);	//��λDMAͨ������
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)(&(USARTx->DR));//�������ַ��USARTx���ݼĴ�����ַ
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)USARTx_RxBuffer;	//�ڴ����ַ���������ݻ�������ַ
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;	//������Դ������
	DMA_InitStruct.DMA_BufferSize = USARTx_RxBUFFERSIZE;	//��������С������Ŀ�ĵش�С
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//����ĵ�ַ������
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;	//�ڴ�ĵ�ַ����
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݴ�СΪ8λ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�ڴ����ݴ�СҲΪ8λ
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;	//����ģʽ���շ���һ�ξͲ����շ��˶�����ѭ���շ�
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;	//���ȼ�����
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;	//���ô洢�����洢��ģʽ����Ϊ���������赽�洢��ģʽ
	DMA_Init(USARTx_Rx_DMA_CH,&DMA_InitStruct);	//��ʼ��DMAͨ��
	DMA_Cmd(USARTx_Rx_DMA_CH,ENABLE);	//ʹ�ܴ��ڽ���DMAͨ��	
	//3.3��ʼ��DMA��ӦUSARTx���͵�ͨ��
	DMA_DeInit(USARTx_Tx_DMA_CH);	//��λDMAͨ������
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)USARTx_TxBuffer;	//�ڴ����ַ���������ݻ�������ַ
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;	//������������
	DMA_InitStruct.DMA_BufferSize = USARTx_TxBUFFERSIZE;	//��������С������Ŀ�ĵش�С
	DMA_Init(USARTx_Tx_DMA_CH,&DMA_InitStruct);	//��ʼ��DMAͨ��
	DMA_ITConfig(USARTx_Tx_DMA_CH,DMA_IT_TC,ENABLE);	//��������ɱ�־λ���жϰ�
	DMA_Cmd(USARTx_Tx_DMA_CH,DISABLE);	//�ȹرմ��ڷ���DMAͨ��
//-----------------4.��ʼ��USARTx-----------------------------------------
	RCC_APB2PeriphClockCmd(USARTx_CLOCK, ENABLE);	//��USARTxʱ��
	USART_InitStruct.USART_BaudRate = BaudRate;//���ڲ�����
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStruct.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USARTx,&USART_InitStruct); //��ʼ������
	USART_ITConfig(USARTx,USART_IT_IDLE,ENABLE);//������������߿��б�־λ���жϰ�
	USART_DMACmd(USARTx,USART_DMAReq_Rx,ENABLE);	//ʹ�ܽ���DMA����
	USART_DMACmd(USARTx,USART_DMAReq_Tx,ENABLE);	//ʹ�ܷ���DMA����
	USART_Cmd(USARTx, ENABLE);	//ʹ�ܴ���	
}
/*---------------------------------------------------------------------------
��������USARTx_IRQHandler
���ܣ�����USARTx�ж���Ӧ����
�����������
����ֵ����
---------------------------------------------------------------------------*/
void USARTx_IRQHandler(void)
{
	if(USART_GetITStatus(USARTx,USART_IT_IDLE) == SET)
	{
		USART_ClearITPendingBit(USARTx,USART_IT_IDLE);
		USART_ReceiveData(USARTx);
		DMA_Cmd(USARTx_Rx_DMA_CH,DISABLE);	//�ȹرս���DMA�������
		ReceivedDataSize = USARTx_RxBUFFERSIZE - 
							 DMA_GetCurrDataCounter(USARTx_Rx_DMA_CH);
							 //��ȡ���յ����ݵĴ�С
		DMA_SetCurrDataCounter(USARTx_Rx_DMA_CH,USARTx_RxBUFFERSIZE);
		DMA_Cmd(USARTx_Rx_DMA_CH,ENABLE);	//����DMA����ͨ��
		USARTx_Stat |= USART_STATE_RECEIVED;	//���ý�����ɱ�־λ
	}
}
/*---------------------------------------------------------------------------
��������USARTx_CheckDataArrival
���ܣ�����Ƿ��յ�������
�����������
����ֵ��uint16_t - 0��û�յ����ݣ�other:�յ����ݣ������յ����ݵĳ���
---------------------------------------------------------------------------*/
uint16_t USARTx_CheckDataArrival(void)
{
	if(USARTx_Stat & USART_STATE_RECEIVED)
	{
		USARTx_Stat &= ~USART_STATE_RECEIVED;	//������ճɹ���־λ
		return ReceivedDataSize;
	}
	else
		return 0;	//û�յ����� ����0
}
/*---------------------------------------------------------------------------
��������Tx_DMA_CH_IRQHandler
���ܣ�DMA���ڷ���ͨ���ж���Ӧ����
�����������
����ֵ����
---------------------------------------------------------------------------*/
void Tx_DMA_CH_IRQHandler(void)
{
	if(DMA_GetITStatus(Tx_DMA_CH_IT_TC) == SET)	//�������ͨ���������
	{
		DMA_ClearITPendingBit(Tx_DMA_CH_IT_TC);
		DMA_Cmd(USARTx_Tx_DMA_CH,DISABLE);	//�رշ���DMAͨ��
	}
}
/*---------------------------------------------------------------------------
��������USARTx_StartDMATransmit
���ܣ���ʼһ��DMA����
���������uint16_t DatSize - Ҫ���͵����ݳ���
����ֵ����
---------------------------------------------------------------------------*/
void USARTx_StartDMATransmit(uint16_t DatSize)
{
	if(DatSize > USARTx_TxBUFFERSIZE)
		DatSize = USARTx_TxBUFFERSIZE;	//����DatSize���������ͻ�������С
	DMA_SetCurrDataCounter(USARTx_Tx_DMA_CH,DatSize);//����DMA����������
	DMA_Cmd(USARTx_Tx_DMA_CH,ENABLE);//����DMA����
}
