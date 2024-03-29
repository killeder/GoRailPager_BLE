﻿/*-----------------------------------------------------------------------
*@file     HW_TIM_Timebase.h
*@brief    定时器中断配置
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/08/22
-----------------------------------------------------------------------*/
#ifndef _HW_TIMBASE_H
#define _HW_TIMBASE_H

void TIM_TimeBase_Init(void);

#define TIMEBASE_TIM_CLK 		RCC_APB1Periph_TIM14
#define TIMEBASE_TIM 			TIM14
#define TIMEBASE_PERIOD			100
#define TIMEBASE_CNT_FREQ		10000	//(1/10000)*(99+1)=0.01s=10ms
#define TIMEBASE_IRQn			TIM14_IRQn
#define TIMEBASE_IRQHandler 	TIM14_IRQHandler

#endif
