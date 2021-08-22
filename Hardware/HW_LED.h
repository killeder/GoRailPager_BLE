/*-----------------------------------------------------------------------
*@file     HW_LED.h
*@brief    LED的有关定义和初始化
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/08/20
-----------------------------------------------------------------------*/
#ifndef HW_LED_H
#define HW_LED_H

void LED_Init(void);	//初始化与LED连接的GPIO端口

//定义与LED和蜂鸣器连接的GPIO端口，修改下列宏即可改变控制LED和蜂鸣器的引脚
#define STATUS_LED_PORT		GPIOA
#define STATUS_LED_CLOCK	RCC_AHBPeriph_GPIOA
#define STATUS_LED_PIN		GPIO_Pin_1

//LED的操作
#define STATUS_LED_ON() STATUS_LED_PORT->BSRR = STATUS_LED_PIN
#define STATUS_LED_OFF() STATUS_LED_PORT->BRR = STATUS_LED_PIN
#define STATUS_LED_TOGGLE() STATUS_LED_PORT->ODR ^= STATUS_LED_PIN

typedef enum{BLINK_FAST = 0,BLINK_SLOW,BLINK_ONCE,BLINK_OFF,BLINK_UNDEFINED} BLINK_MODE;//状态指示灯闪烁模式
extern BLINK_MODE StatusBlinkMode;

void LedBlinkHandler(void); //Handler of led blinking, called by Timebase

#endif
