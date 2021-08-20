/*-----------------------------------------------------------------------
*@file     HW_TM1640_LED.h
*@brief    Dynamically scanned 7-seg led displayer driven by TM1640
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/02/12
-----------------------------------------------------------------------*/
#ifndef HW_TM1640_LED_H
#define HW_TM1640_LED_H

extern uint8_t DisplayBuffer[2][8];	//displsy buffer holding segment data
extern const uint8_t SegPattern[];//segment pattern for number 0-9,blank and "-"
//To show digit point, should set MSB of x, which is connected to DP pin 
//while 7-Seg LED is common cathode
#define SHOW_DIGPOINT(x)	((x) |= 0x80)
#define PATTERN_BLANK	10
#define PATTERN_MINUS	11
#define PATTERN_U		12
#define PATTERN_d		13

void LED7Seg_Init(void);	//LED displayer init.
void LED7Seg_Update(void);	//update display contents
void LED7Seg_SetBright(uint8_t level);//set brightness level

#endif
