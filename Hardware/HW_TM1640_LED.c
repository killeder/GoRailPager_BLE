/*-----------------------------------------------------------------------
*@file     HW_TM1640_LED.c
*@brief    Dynamically scanned 7-seg led displayer driven by TM1640
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2021/02/12
*@notes

1.The 7-seg leds are [Common cathode]
2.About connection between segment driver and led:
	Segment a~dp -> SEG 1~8 -> DDRAM bit0~bit7
3.About connection between digit driver and led:
	Digit row 1:(physical left)	1 	2 	3 	4 	5 	6 	7 	8	(physical right)
	Digit row 2:(physical left)	9  10  11  12  13  14  15  16	(physical right)
	Digit 1~16 -> GRID 1~16 -> DDRAM address 0xC0~0xCF
-----------------------------------------------------------------------*/
#include "Hardware.h"

#define SDATA_GPIO_PORT		GPIOB	//GPTO port of TM1640 serial data
#define SDATA_GPIO_CLOCK	RCC_AHBPeriph_GPIOB	//clock of GPIO port
#define SDATA_PIN			GPIO_Pin_3	//GPIO pin of TM1640 serial data
#define SDATA_High()		SDATA_GPIO_PORT->BSRR = SDATA_PIN //set serial data high
#define SDATA_Low()			SDATA_GPIO_PORT->BRR = SDATA_PIN //set serial data low

#define SCLK_GPIO_PORT		GPIOB	//GPTO port of TM1640 serial clock
#define SCLK_GPIO_CLOCK		RCC_AHBPeriph_GPIOB	//clock of GPIO port
#define SCLK_PIN			GPIO_Pin_4	//GPIO pin of TM1640 serial clock
#define SCLK_High()			SCLK_GPIO_PORT->BSRR = SCLK_PIN //set serial clk high
#define SCLK_Low()			SCLK_GPIO_PORT->BRR = SCLK_PIN //set serial clk low

/*---------------------------------------------------------------------
Display buffer: holding 2 rows and 8 columns of 7-Seg LED display data.
The 7-seg leds are [Common cathode]  
-----------------------------------------------------------------------*/
uint8_t DisplayBuffer[2][8] = {0};
/*---------------------------------------------------------------------
Segment pattern of 7-Seg LED display data.The 7-seg leds are [Common cathode]
for number 0-9,blank and "-"  
-----------------------------------------------------------------------*/
const uint8_t SegPattern[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x67,//0-9
								0x00,0x40,0x3e,0x5e};//" ","-","U","d"
/*-----------------------------------------------------------------------
*@brief		TM1640 Serial Start
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void TM1640_Start(void)
{
	SCLK_High();	//set data to low while clk is high
	Delay_us(2);
	SDATA_High();
	Delay_us(2);
	SDATA_Low();
	Delay_us(2);
	SCLK_Low();
}
/*-----------------------------------------------------------------------
*@brief		TM1640 Serial Stop
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void TM1640_Stop(void)
{
	SCLK_Low();	//set clk to high while data is low
	SDATA_Low();
	SCLK_High();
	Delay_us(2);
	SDATA_High();
}
/*-----------------------------------------------------------------------
*@brief		Send one byte to serial wire
*@param		uint8_t ByteData - Byte data to send
*@retval	none
-----------------------------------------------------------------------*/
static void SendOneByte_TM1640(uint8_t ByteData)
{
	for(uint8_t i = 0;i < 8;i++)
	{
		SCLK_Low();
		Delay_us(2);	

		if(ByteData & 0x01)
			SDATA_High();
		else
			SDATA_Low();

		SCLK_High();
		Delay_us(2);
		ByteData >>= 1;
	}
	SCLK_Low();
}
/*-----------------------------------------------------------------------
*@brief		TM1640 GPIO config
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void LED7Seg_GPIOConfig(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(SDATA_GPIO_CLOCK|SCLK_GPIO_CLOCK,ENABLE);//enable GPIO clocks
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = SDATA_PIN;
	GPIO_Init(SDATA_GPIO_PORT,&GPIO_InitStructure);;

	GPIO_InitStructure.GPIO_Pin = SCLK_PIN;
	GPIO_Init(SCLK_GPIO_PORT,&GPIO_InitStructure);

	SCLK_Low();//make serial clock low
}
/*-----------------------------------------------------------------------
*@brief		TM1640 display init.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void LED7Seg_Init(void)
{
	LED7Seg_GPIOConfig();	//config GPIO of TM1640

	TM1640_Start();
	SendOneByte_TM1640(0x80);//turn off display
	TM1640_Stop();

	TM1640_Start();
	SendOneByte_TM1640(0x40);//normal mode , address auto increase
	TM1640_Stop();

	TM1640_Start();
	SendOneByte_TM1640(0x88|0x07);//turn on display, bright set to highest level
	TM1640_Stop();
}
/*-----------------------------------------------------------------------
*@brief		update display contents
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void LED7Seg_Update(void)
{
	TM1640_Start();
	SendOneByte_TM1640(0xC0);//set start address of DDRAM
	for(uint8_t i=0;i<8;i++)
	{
		SendOneByte_TM1640(DisplayBuffer[0][i]);
	}
	for(uint8_t i=0;i<8;i++)
	{
		SendOneByte_TM1640(DisplayBuffer[1][i]);
	}
	TM1640_Stop();
}
/*-----------------------------------------------------------------------
*@brief		set brightness level
*@param		level - 0~7
*@retval	none
-----------------------------------------------------------------------*/
void LED7Seg_SetBright(uint8_t level)
{
	TM1640_Start();
	SendOneByte_TM1640(0x88|(level&0x07));//turn on display, set bright level
	TM1640_Stop();	
}
