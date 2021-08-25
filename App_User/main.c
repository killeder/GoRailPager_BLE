/*-----------------------------------------------------------------------
*@file     main.c
*@brief    GoRailPager_BLE main procedure
*@author   Xie Yinanan(xieyingnan1994@163.com)
*@version  1.0
*@date     2021/08/21
-----------------------------------------------------------------------*/
#include "GoRailPager_BLE.h"

enum _sys_status{SYS_NORMAL,SYS_ALARM} SystemStatus;//enum of system status
/*-----------------------------------------------------------------------
*@brief		Intializations of several basic hardwares
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void Basic_Hardware_Init(void)
{
	Delay_Init();		//Initialize of "SystemTick" timer for delay
	HW_USART1_Init(115200);	//Init UART1 to 115200,N,8,1
	HW_USART_DMA_Init(9600);//Init UART2 with DMA for connecting BLE module
	LED_Init();	//Initialize of status LED GPIO
	TIM_TimeBase_Init();	//Init timer for time-base use.
	ADConvert_Init();	//Init of battery voltage A/D converter
}
/*-----------------------------------------------------------------------
*@brief		Intializations of extended hardwares
*@param		none
*@retval	True - Init extende hardwares OK
			False - Init encountered problems
-----------------------------------------------------------------------*/
static bool Ex_Hardware_Init(void)
{
	do{
		if(!Radio_Init());	//Detect radio CC1101 and initialize
			break;
		if(!BLE_CheckPresence());	//check HC-08 BLE module presence
			break;
		if(!Battery_CheckVoltage());	//check battery voltage
			break;
		return true;
	}while(0);
	//if any of above initializing procedures failed,
	//program will jump to here and function will return false
	return false;
}
/*-----------------------------------------------------------------------
*@brief		main procedure: firmware program entrance
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
int main(void)
{	
	Basic_Hardware_Init();	//Initialize basic hardwares
	//if any of the extended hardware failed to be initialized
	if(!Ex_Hardware_Init())
	{
		StatusBlinkMode = BLINK_FAST;	//set status led fast blink
		MSG("Fatal error, system halted!\r\n");//print "halt" message
		while(true);	//infinite loop to halt microcontroller
	}
	UART_ShowBuildInfo();	//Print build info on debug serialport
	//if anything above went okay, then set system status to "normal"
	//as well as start to receive LBJ messages
	CC1101_StartReceive(RF_Rx_Callback);//Start receiving...
	StatusBlinkMode = BLINK_SLOW;	//set status led slowly blink
	SystemStatus = SYS_NORMAL;	//set system status to "Normal"

	while(true)	//main procedure loop...
	{
		//check radio rx only if system status is "normal"			
		if(SystemStatus == SYS_NORMAL)
		{
			if(bRadioDataArrival)
			{
				//Radio data rx and transfer LBJ message			{
				RF_RxData_Handler();//Handle rx data on data arrival
				//Clear rx data arrival flag after handle it
				bRadioDataArrival = false;
			}
		}
		//following procedures will be done neither system status is "normal"
		//nor the status is "alarm"

		//if it's time to check battery
		if(bCheckBattery_Flag)//this flag was set in timebase module
		{
			if(IS_BATTERY_LOW())
			{
				//if battery level is low as well as current state is normal
				//set system state to alarm and status led to fast blink
				if(SystemStatus == SYS_NORMAL)
				{
					SystemStatus = SYS_ALARM;
					StatusBlinkMode = BLINK_FAST;
					BLE_SendBatteryAlarm();//send battery alarm message via BLE
					MSG("Battery low, alarm!\r\n");
				}
			}
			else
			{
				//if battery is ok as well as current state is "alarm"
				//resume system state to normal and status led to slow blink 
				if(SystemStatus == SYS_ALARM)
				{
					SystemStatus = SYS_NORMAL;
					StatusBlinkMode = BLINK_SLOW;
					MSG("Battery OK, resume to normal!\r\n");
				}
			}
			bCheckBattery_Flag = false;
		}
		//if serial port data arrived, parse serial command
		if(bUSART1_RxFinish)
		{
			UART_ParseCmd(USART1_RxBuffer);//parse serial command
			HW_USART1_RxClear();	//clear buffer after parse
		}
		//if BLE data(via serial port) arrived, parse BLE command
		if(bUSART_DMA_RxFinish)
		{
			BLE_ParseCmd(USART_DMA_RxBuffer);//parse BLE command
			HW_USART_DMA_RxClear();//clear buffer after parse
		}
	}
}
