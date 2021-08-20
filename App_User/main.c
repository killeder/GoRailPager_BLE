/*-----------------------------------------------------------------------
*@file     main.c
*@brief    GoRailPager_BLE main procedure
*@author   Xie Yinanan(xieyingnan1994@163.com)
*@version  1.0
*@date     2021/08/20
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
	LED_Init();	//Initialize of status LED GPIO
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

	//Detect radio CC1101 and initialize...
	if(Radio_Init())	//if CC1101 init OK
	{
		CC1101_StartReceive(Rf_Rx_Callback);//Start receiving...
		StatusBlinkMode = BLINK_SLOW;	//set status led slowly blink
		while(true)
		{			
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
		StatusBlinkMode = BLINK_FAST;	//set status led fast blink
		while(true)
		{

		}
	}
}
