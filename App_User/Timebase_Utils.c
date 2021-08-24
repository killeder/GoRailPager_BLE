/*-----------------------------------------------------------------
*@file     Timebase_Utils.c
*@brief    Human interface and displaying utilities
*@author   Xie Yingnan(xieyingnan1994@163.com)
*@version  1.0
*@date     2021/08/22
-----------------------------------------------------------------------*/
#include "GoRailPager_BLE.h"
/*-----------------------------------------------------------------------
*@brief		Timer IRQ for providing Time-base for indicators use
*@detail 	Timer IRQ cycle is determined when Timer init.
*         	1ms in this program.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void TIMEBASE_IRQHandler(void)
{
	static uint16_t timebase_cnt = 0;

	if(TIM_GetITStatus(TIMEBASE_TIM,TIM_IT_Update)!=RESET)
	{
		LedBlinkHandler();	//for led blink, every 10ms	
		if(timebase_cnt%200 == 0)//trig battery check every 2000ms
		{
			bCheckBattery_Flag = true;
		}
		++timebase_cnt;
	}
	TIM_ClearITPendingBit(TIMEBASE_TIM,TIM_IT_Update);
}