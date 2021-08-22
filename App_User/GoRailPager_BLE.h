/*-----------------------------------------------------------------------
*@file     GoRailPager_BLE.h
*@brief    Project global include file, should be included in 
*          application and system layer(aka. Hi-Layers).
*@author   Xie Yingnan(xieyingnan1994@163.com��
*@version  1.0
*@date     2021/08/21
-----------------------------------------------------------------------*/
#ifndef GORAILPAGER_BLE_H
#define GORAILPAGER_BLE_H
/*-------------Chip layer supporting-------------*/
#include "stm32f0xx.h"
/*--------------Standard library of C------------*/
#include <stdio.h>	//printf...
#include <string.h>	//memcpy...
#include <stdint.h>	//uint8_t...
#include <stdbool.h>//C99:true...false...
#include <stdarg.h> //C99:VA_ARGS...
#include <stdlib.h> //atoi...
/*-----------Hardware-Layer Hardwares----------*/
#include "Hardware.h"
/*-----------System-Layer supporting------------*/
#include "bit.h"	//bit operations
#include "delay.h"	//delays
#include "POCSAG_ParseLBJ.h" //POCSAG Parse fot LBJ message
/*-----------App-Layer  utilities---------------*/
#include "Radio_Utils.h" //800MHz RF Radio receiving utilities
#include "Transfer_Utils.h" //Serial port and BLE transfer utilities
#include "Timebase_Utils.h"	//Timer base for interval tasks
#include "Battery_Utils.h"	//Battery voltage checking utilities
/*-----------Other global definations-----------*/
#ifndef MSG
#define MSG(...) Trace_printf(__VA_ARGS__)	//define debug printing macro
#endif
#ifdef DEBUG_MSG_ON
	    #define DEBUG_MSG(...) Trace_printf(__VA_ARGS__)
#else
		#define DEBUG_MSG(...)
#endif
/* Version and build info strings */
#define APP_NAME_STR "GoRailPager_BLE"
#define VERTION_STR "1.0.0"
#define BUILD_DATE_STR __DATE__
#define BUILD_TIME_STR __TIME__
#endif
