/*-----------------------------------------------------------------------
*@file     HW_LedAndBuzzer.h
*@brief    LED和蜂鸣器的有关定义和初始化
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#ifndef HW_LED_BUZZER_H
#define HW_LED_BUZZER_H

void LedAndBuzzer_Init(void);	//初始化与LED和蜂鸣器连接的GPIO端口

//定义与LED和蜂鸣器连接的GPIO端口，修改下列宏即可改变控制LED和蜂鸣器的引脚
#define STATUS_LED_PORT		GPIOB
#define STATUS_LED_CLOCK	RCC_AHBPeriph_GPIOB
#define STATUS_LED_PIN		GPIO_Pin_5

#define DECODE_LED_PORT		GPIOB
#define DECODE_LED_CLOCK	RCC_AHBPeriph_GPIOB
#define DECODE_LED_PIN		GPIO_Pin_6

#define BUZZER_PORT		GPIOB
#define BUZZER_CLOCK	RCC_AHBPeriph_GPIOB
#define BUZZER_PIN		GPIO_Pin_7

//LED和蜂鸣器的操作
#define STATUS_LED_ON() STATUS_LED_PORT->BSRR = STATUS_LED_PIN
#define STATUS_LED_OFF() STATUS_LED_PORT->BRR = STATUS_LED_PIN
#define STATUS_LED_TOGGLE() STATUS_LED_PORT->ODR ^= STATUS_LED_PIN

#define DECODE_LED_ON() DECODE_LED_PORT->BSRR = DECODE_LED_PIN
#define DECODE_LED_OFF() DECODE_LED_PORT->BRR = DECODE_LED_PIN
#define DECODE_LED_TOGGLE() DECODE_LED_PORT->ODR ^= DECODE_LED_PIN

#define BUZZER_ON() BUZZER_PORT->BSRR = BUZZER_PIN
#define BUZZER_OFF() BUZZER_PORT->BRR = BUZZER_PIN
#define BUZZER_TOGGLE() BUZZER_PORT->ODR ^= BUZZER_PIN

typedef enum{BEEP_ONCE = 0,DBL_BEEP,BEEP_OFF} BEEPER_MODE;	//蜂鸣器蜂鸣模式
extern BEEPER_MODE BeeperMode;
typedef enum{BLINK_FAST = 0,BLINK_SLOW,BLINK_ONCE,BLINK_OFF,BLINK_UNDEFINED} BLINK_MODE;//状态指示灯闪烁模式
extern BLINK_MODE StatusBlinkMode,DecodeBlinkMode;

#endif
