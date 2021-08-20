/*-----------------------------------------------------------------------
*@file     HW_TIM_Timebase.c
*@brief    定时器中断配置
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#include "Hardware.h"
	
/*-----------------------------------------------------------------------
*@brief		时基定时器设置中断配置
*@retval	无
-----------------------------------------------------------------------*/
void TIM_TimeBase_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue;

	RCC_APB1PeriphClockCmd(TIMEBASE_TIM_CLK,ENABLE);	//打开时钟
	//计算计数时钟分频系数，TIMEBASE_CNT_FREQ表示计数频率
	PrescalerValue = (uint16_t)(SystemCoreClock / TIMEBASE_CNT_FREQ) - 1;
	//初始化定时器时基，TIMEBASE_PERIOD表示溢出周期
	TIM_TimeBaseStructure.TIM_Period = TIMEBASE_PERIOD - 1;
  	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMEBASE_TIM, &TIM_TimeBaseStructure);
	//设置中断源为溢出中断
	TIM_ITConfig(TIMEBASE_TIM,TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIMEBASE_TIM, TIM_IT_Update);
	//设置中断
	NVIC_InitStructure.NVIC_IRQChannel = TIMEBASE_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//定时器使能
	TIM_Cmd(TIMEBASE_TIM,ENABLE);
}
