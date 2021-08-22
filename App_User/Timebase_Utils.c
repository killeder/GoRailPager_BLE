/*-----------------------------------------------------------------
*@file     Transfer_Utils.c
*@brief    Human interface and displaying utilities
*@author   Xie Yingnan(xieyingnan1994@163.com)
*@version  1.0
*@date     2020/08/28
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
	static uint8_t timebase_cnt = 0;

	if(TIM_GetITStatus(TIMEBASE_TIM,TIM_IT_Update)!=RESET)
	{
		if(timebase_cnt%10 == 0)//handle led and buzzer every 10ms
		{
			LedBlinkHandler();	//for led blink	
		}
		++timebase_cnt;
	}
	TIM_ClearITPendingBit(TIMEBASE_TIM,TIM_IT_Update);
}