/*-----------------------------------------------------------------------
*@file     main.c
*@brief    GoRailPager_BLE main procedure
*@author   Xie Yinanan(xieyingnan1994@163.com)
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#include "GoRailPager_BLE.h"
/*-----------------------------------------------------------------------
*@brief		Intializations of several basic hardwares
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void Basic_Hardware_Init(void)
{
	Delay_Init();		//Initialize of "SystemTick" timer for delay
	HW_USART1_Init(115200);	//Initialize Serialport UART1 to 115200,N,8,1
	LED7Seg_Init();	//TM1640 LED-7seg driver init.
	LedAndBuzzer_Init();	//Initialize of Led and Buzzer GPIOs
	MultiKeys_Init();	//init keyboard and GPIOs.
	TIM_TimeBase_Init();	//Init timer for time-base use.
}
/*-----------------------------------------------------------------------
*@brief		main procedure: entrance of this program.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
int main(void)
{	
	Basic_Hardware_Init();	//Initialize basic hardwares
	ShowBuildInfo();	//Print build info on serialport
	ShowSplashScreen();	//Show splash screen

	//Detect radio CC1101 and initialize...
	if(Radio_Init())	//if CC1101 init OK
	{
		CC1101_StartReceive(Rf_Rx_Callback);//Start receiving...
		LED_SwitchScreen(SCREEN_NORMAL);	//show normal screen on led displayer
		StatusBlinkMode = BLINK_SLOW;	//set status led slowly blink
		BeeperMode = BEEP_ONCE;	//beep once
		while(true)
		{
			KeyProcess();	//key scanning
			if(bit_IsTrue(Key_Trig,CONFIG_KEY)) //if "Config" button pressed
			{	
				//toggle "enable message beep" flag
				bEnableMessageBeep = !bEnableMessageBeep;
				if(bEnableMessageBeep)
					DecodeBlinkMode = BLINK_ONCE;
				//blink "decode" led once after enabled message beep
			}
			if(bit_IsTrue(Key_Trig,MODE_KEY)) //if "mode" button pressed
			{	
				static uint8_t level = 1;
				if(++level>2)
					level = 0;
				switch(level)
				{
					case 0:	LED7Seg_SetBright(1);	break;
					case 1:	LED7Seg_SetBright(2);	break;
					case 2:	LED7Seg_SetBright(7);	break;
					default: break;
				}
			}			
			//Radio data rx and LBJ message show
			if(bRadioDataArrival)
			{
				RxData_Handler();//Handle rx data on data arrival
				//Clear rx data arrival flag after handle it
				bRadioDataArrival = false;
			}
		}
	}
	else	//if CC1101 init failed
	{
		LED_SwitchScreen(SCREEN_FAULT);	//show fault screen on led displayer
		StatusBlinkMode = BLINK_FAST;	//set status led fast blink
		while(true)
		{
			static uint8_t delay_cnt = 0;
			Delay_ms(1);
			if(++delay_cnt == 200)//double beep every 200ms
			{
				BeeperMode = DBL_BEEP;
				delay_cnt = 0;
			}
		}
	}
}
