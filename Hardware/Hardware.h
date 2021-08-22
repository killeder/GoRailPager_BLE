/*-----------------------------------------------------------------------
*@file     Hardware.h
*@brief    底层硬件的总体头文件,在需要包含硬件头文件时包含此文件即可。
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#ifndef HARDWARE_H
#define HARDWARE_H
/*-------------芯片级支持库-------------*/
#include "stm32f0xx.h"
/*-------------C语言标准库--------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
/*---------引用自外部的其他模块---------*/
#include "delay.h"	//基于Systick的公用延时功能，在Sys/Utils层中定义
/*------各个底层硬件驱动程序头文件集合----*/
#include "HW_USART.h"	//串行口
#include "HW_TIM_Timebase.h"	//作为时基的定时器
#include "HW_LED.h"	//指示灯
#include "HW_RADIO_CC1101.h"	//CC1101无线芯片驱动程序
#include "HW_ADConvert.h"
#include "HW_USART_DMA.h"
/*-----------宏定义和一些参数------------*/
#define CRIS_ENTER()	__set_PRIMASK(1)	//进入临界区，关闭总中断
#define CRIS_EXIT()		__set_PRIMASK(0)	//退出临界区，开启总中断
#endif
