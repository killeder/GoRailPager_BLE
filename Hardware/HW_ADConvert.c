/*-----------------------------------------------------------------------
*@file     HW_ADConvert.c
*@brief    模/数转换器配置
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#include "Hardware.h"
//AD输入端口定义
#define AIN_GPIO_PIN	GPIO_Pin_0 
#define AIN_GPIO_PORT	GPIOA
#define AIN_GPIO_CLOCK	RCC_AHBPeriph_GPIOA
#define AIN_ADC_CHANNEL	ADC_Channel_0	//AD通道，端口更改后通道也要改
/*-----------------------------------------------------------------------
*@brief		模/数转换器配置
*@detail 	单通道，单次触发，查询模式
*@param		无
*@retval	无
-----------------------------------------------------------------------*/
void ADConvert_Init(void)
{
	ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(AIN_GPIO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = AIN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(AIN_GPIO_PORT, &GPIO_InitStructure);

	ADC_StructInit(&ADC_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	ADC_ClockModeConfig(ADC1,ADC_ClockMode_SynClkDiv4);//ADC时钟=48/4=12MHz
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //单次触发
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_ChannelConfig(ADC1,AIN_ADC_CHANNEL,ADC_SampleTime_55_5Cycles);

	ADC_GetCalibrationFactor(ADC1);
	ADC_Cmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));
}
/*-----------------------------------------------------------------------
*@brief		启动一次AD转换，读取AIN引脚上的电压
*@detail 	电压值以毫伏表示，并乘以10000，最大值33000
*@param		无
*@retval	以0.1毫伏表示的AIN引脚电压,转换为整数,30654表示3.0654V
-----------------------------------------------------------------------*/
uint16_t Get_AIN_Voltage(void)
{
	uint16_t ADC_Raw, ADC_Converted;

	ADC_StartOfConversion(ADC1);//触发一次AD转换
	//等待转换完成
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	//计算转换结果
	ADC_Raw = ADC_GetConversionValue(ADC1);
	ADC_Converted = (ADC_Raw*33000) / 0xFFF;
	return ADC_Converted;
}
