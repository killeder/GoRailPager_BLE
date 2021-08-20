/*-----------------------------------------------------------------
*@file     Human_Interface.c
*@brief    Human interface and displaying utilities
*@author   Xie Yingnan(xieyingnan1994@163.com)
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#include "GoRailPager_BLE.h"

/*-----------------------------------------------------------------------
*@brief		Print build info and version info via seiralport
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ShowBuildInfo(void)
{
	MSG("%s Version%s (Build Date %s %s)\r\n",APP_NAME_STR,
		      VERTION_STR,BUILD_DATE_STR,BUILD_TIME_STR);//串口打印版本信息
	MSG("Xie Yingnan Works.<xieyingnan1994@163.com>\r\n");
}
/*-----------------------------------------------------------------------
*@brief		Show decoded LBJ info on LCD1602
*@param		LBJ_Msg - pointer to text LBJ message
*@retval	none
-----------------------------------------------------------------------*/
void ShowMessageLBJ(POCSAG_RESULT* POCSAG_Msg)
{
	//Store traincode/speed/milemark in led segment pattern
	uint8_t LBJ_Info[15] = {0};
	/*	string "POCSAG_Msg->txtMsg" normally holds 15 characters. i.e.
		"47963<sp>100<sp>23456". Symbol <sp> signifies space, aka.'\0x20'.
		the "-" character could also appear in above string, sinifying 
		non-available of the corresponding infomation.
		So we should turn the ASCII formated LBJ message to LED pattern,
		we only intercept the heading 15 characters of "txtMsg" string.
	*/

}
/*-----------------------------------------------------------------------
*@brief		Show RSSI level(1-5) according to rssi value received
*@param		rssi value e.g. -74.5
*@retval	none
-----------------------------------------------------------------------*/
void ShowRSSILevel(float rssi)
{
	uint8_t level;

	if(rssi >= -85.0)
		level = 5;	//>=85dBm
	else if(rssi >= -90.0)
		level = 4;	//-90dBm ~ -85dBm
	else if(rssi >= -95.0)
		level = 3;	//-95dBm ~ -90dBm
	else if(rssi >= -100.0)
		level = 2;	//-100dBm ~ -95dBm
	else if(rssi >= -105.0)
		level = 1;	//-105dBm ~ -100dBm
	else
		level = 0;	//<-105dBm
}
/*-----------------------------------------------------------------------
*@brief		Handler of led blinking, called by Timebase
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void LedBlinkHandler(void)
{
	static uint8_t cnt_status_blink = 0;
	static uint8_t cnt_decode_blink = 0;
	//status led blink
	switch(StatusBlinkMode)	//status led
	{
	case BLINK_FAST:
		if(++cnt_status_blink >= 10)	//cycle100ms,50%duty
		{ STATUS_LED_TOGGLE(); cnt_status_blink = 0; }
		break;
	case BLINK_SLOW:
		if(cnt_status_blink <= 20) {STATUS_LED_ON();}	//on 200ms
		else {STATUS_LED_OFF();}			//off 1480ms
		if(++cnt_status_blink >= 150)				//cycle 1500ms
			cnt_status_blink = 0;
		break;
	case BLINK_OFF:
		STATUS_LED_OFF();
		cnt_status_blink = 0;
		StatusBlinkMode = BLINK_UNDEFINED;
		break;
	default:		
		break;
	}
}
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
