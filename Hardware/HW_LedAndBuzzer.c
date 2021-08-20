/*-----------------------------------------------------------------------
*@file     HW_LedAndBuzzer.c
*@brief    LED和蜂鸣器的有关定义和初始化
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#include "Hardware.h"

BEEPER_MODE BeeperMode = BEEP_OFF;	//蜂鸣器的蜂鸣方式
BLINK_MODE StatusBlinkMode = BLINK_UNDEFINED;	//状态指示灯闪烁模式
BLINK_MODE DecodeBlinkMode = BLINK_UNDEFINED;	//解码成功指示灯闪烁模式
/*---------------------------------------------------------------------------
函数名：IIC_GPIOConfig
功能：初始化与LED和蜂鸣器连接的GPIO端口
---------------------------------------------------------------------------*/
void LedAndBuzzer_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(STATUS_LED_CLOCK|DECODE_LED_CLOCK|
							BUZZER_CLOCK,ENABLE);//打开各端口时钟
	
	GPIO_InitStructure.GPIO_Pin = STATUS_LED_PIN;//选择LED1引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//设定速率2MHz低速
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//不上拉
	GPIO_Init(STATUS_LED_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DECODE_LED_PIN;//选择LED2引脚
	GPIO_Init(DECODE_LED_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;//选择Buzzer引脚
	GPIO_Init(BUZZER_PORT,&GPIO_InitStructure);

	STATUS_LED_OFF();
	DECODE_LED_OFF();
	BUZZER_OFF();//初始化后关闭2个LED和蜂鸣器
}
