/*-----------------------------------------------------------------------
*@file     HW_ADConvert.h
*@brief    模/数转换器配置
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#ifndef HW_ADCONVERT_H
#define HW_ADCONVERT_H

void ADConvert_Init(void);//初始化ADC
uint16_t Get_AIN_Voltage(void);//启动一次AD转换，读取AIN引脚上的电压

#endif
