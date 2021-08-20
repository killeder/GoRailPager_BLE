/*-----------------------------------------------------------------
*@file     Human_Interface.c
*@brief    Human interface and displaying utilities
*@author   Xie Yingnan(xieyingnan1994@163.com)
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#include "GoRailPager_BLE.h"
const uint8_t FixedScrPattern[][16] = 
{ 	//0.Normal screen
 {0x50,0x7b,0x5f,0x5e,0x6e,0x40,0x40,0x40,	//ready---
  0x40,0x40,0x40,0xc0,0x40,0xc0,0x38,0x40}, //----.--.L-
  	//1.Radio fault screen
 {0x50,0x5f,0x5e,0x04,0x5c,0x40,0x40,0x40,  //radio---
  0x71,0x5f,0x3e,0x38,0x78,0x40,0x40,0x40}, //fault---
    //2.Splash screen: application name
 {0x50,0x5f,0x04,0x38,0x40,0x40,0x3d,0x5c,  //rail--Go
  0x73,0x5f,0x6f,0x7b,0x50,0x00,0x86,0x06}, //pager 1.1
  	//3.Splash screen: author name
 {0x00,0x5b,0x3f,0x5b,0x06,0x40,0x3f,0x4f,  // 2021-03
  0x00,0x00,0x7c,0x74,0x5b,0x50,0x73,0x74}	//  bh2rph
};
volatile bool bEnableMessageBeep = true;

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
	MSG("DE BH2RPH TU VY 73\r\n");
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
	for(uint8_t i=0;i<15;i++)
	{
		char ch;
		if((ch = POCSAG_Msg->txtMsg[i]) == '\0')
			break;
		if(ch >= '0' && ch <= '9')
			LBJ_Info[i] = SegPattern[ch-'0'];
		else if(ch == '\x20')//Space?
			LBJ_Info[i] = SegPattern[PATTERN_BLANK];//fill blank
		else	//"-" or other something?
			LBJ_Info[i] = SegPattern[PATTERN_MINUS];//fill "-"
	}

	memcpy(DisplayBuffer[0],LBJ_Info,5);//fill in train code:5 digits
	memcpy(DisplayBuffer[0]+5,LBJ_Info+6,3);//fill in speed:3 digits
	memcpy(DisplayBuffer[1],LBJ_Info+10,5);//fill in milemark:5 digits
	SHOW_DIGPOINT(DisplayBuffer[1][3]);//show digit point at row2 col4
	//show up/down direction symbol at row 2 column 6
	if(POCSAG_Msg->FuncCode == FUNC_SHANGXING)	
		DisplayBuffer[1][5] = SegPattern[PATTERN_U];//show pattern "U"
	else if(POCSAG_Msg->FuncCode == FUNC_XIAXING)
		DisplayBuffer[1][5] = SegPattern[PATTERN_d];//show pattern "d"
	SHOW_DIGPOINT(DisplayBuffer[1][5]);//show digit point at row2 col6

	LED7Seg_Update();//update display
	if(bEnableMessageBeep)
	{
		switch(POCSAG_Msg->FuncCode)		//beeper beeps one or two times
		{
			case FUNC_XIAXING:
				BeeperMode = BEEP_ONCE;//beep one time
				break;
			case FUNC_SHANGXING:
				BeeperMode = DBL_BEEP;//beep two times
				break;
			default: BeeperMode = DBL_BEEP; break;
		}
	}
	DecodeBlinkMode = BLINK_ONCE;	//blink "decode" LED once
}
/*-----------------------------------------------------------------------
*@brief		show screen on led displayer according to type
*@param		SCREENTYPE type - enum screen type
*@retval	none
-----------------------------------------------------------------------*/
void LED_SwitchScreen(SCREENTYPE type)
{
	switch(type)
	{
		case SCREEN_NORMAL:	//Normal screen display
			memcpy(DisplayBuffer,FixedScrPattern[0],16);
		break;
		case SCREEN_FAULT:	//Radio fault display
			memcpy(DisplayBuffer,FixedScrPattern[1],16);
		break;
		case SCREEN_APPNAME://in splash screen show app name
			memcpy(DisplayBuffer,FixedScrPattern[2],16);
		break;
		case SCREEN_AUTHOR://in splash screen show author name
			memcpy(DisplayBuffer,FixedScrPattern[3],16);
		break;
		default:
		break;
	}
	LED7Seg_Update();//update display
}
/*-----------------------------------------------------------------------
*@brief		Show splash screen on 7-seg led displayer
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ShowSplashScreen(void)
{
	LED_SwitchScreen(SCREEN_APPNAME);
	for(int i=0;i<1500;i++)
		Delay_ms(1);
	LED_SwitchScreen(SCREEN_AUTHOR);
	for(int i=0;i<1500;i++)
		Delay_ms(1);
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

	DisplayBuffer[1][7] = SegPattern[level];
	//fill in displauy buffer,position:row2 column8
	LED7Seg_Update();//update display
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
	//decode led blink
	switch(DecodeBlinkMode)
	{
	case BLINK_ONCE:
		DECODE_LED_ON();
		if(++cnt_decode_blink >= 10)
		{ DECODE_LED_OFF(); DecodeBlinkMode = BLINK_OFF;}
		break;
	default:
		DECODE_LED_OFF();
		cnt_decode_blink = 0;
	}
}
/*-----------------------------------------------------------------------
*@brief		Handler of beeper, called by Timebase
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void BeeperHandler(void)
{
	static uint8_t cnt_beep = 0,cnt_beeptimes = 0;

	switch(BeeperMode)	//beeping
	{
	case BEEP_ONCE:	//beep one time
		BUZZER_ON();
		if(++cnt_beep >= 10)
		{ BUZZER_OFF(); BeeperMode = BEEP_OFF; }
		break;
	case DBL_BEEP: 
		if(cnt_beeptimes < 2)
		{
			if(cnt_beep <= 15) {BUZZER_ON();}	//beep150ms
			else {BUZZER_OFF();}				//stop150ms
			if(++cnt_beep >= 30)				//cycle300ms
			{cnt_beep = 0; cnt_beeptimes++;}
		}
		else
			BeeperMode = BEEP_OFF;	//stop after 2 beeps
		break;
	default:
		BUZZER_OFF();	//default:turn off beeper
		cnt_beep = 0;	//clear counter var
		cnt_beeptimes = 0; //clear beep times countet var
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
			BeeperHandler();	//for buzzer beep	
		}
		++timebase_cnt;
	}
	TIM_ClearITPendingBit(TIMEBASE_TIM,TIM_IT_Update);
}
