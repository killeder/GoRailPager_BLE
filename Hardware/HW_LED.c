/*-----------------------------------------------------------------------
*@file     HW_LED.c
*@brief    LED的有关定义和初始化
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/08/20
-----------------------------------------------------------------------*/
#include "Hardware.h"

BLINK_MODE StatusBlinkMode = BLINK_UNDEFINED;	//状态指示灯闪烁模式
/*---------------------------------------------------------------------------
函数名：LED_Init
功能：初始化与LED连接的GPIO端口
---------------------------------------------------------------------------*/
void LED_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(STATUS_LED_CLOCK,ENABLE);//打开各端口时钟
	
	GPIO_InitStructure.GPIO_Pin = STATUS_LED_PIN;//选择LED1引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//设定速率2MHz低速
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不上拉
	GPIO_Init(STATUS_LED_PORT,&GPIO_InitStructure);

	STATUS_LED_OFF();
}
