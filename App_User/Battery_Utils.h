/**
  ******************************************************************************
  * @file    Battery_Utils.h
  * @author  Xie Yingnan(xieyingnan1994@163.com)
  * @version V1.0.0
  * @date    2021-08-23
  * @brief   Utilities for battery voltage and level check
  ******************************************************************************
**/
#ifndef BATTERY_UTILS_H
#define BATTERY_UTILS_H

uint16_t GetBatteryVoltage(void);   //return battery voltage in mV
uint8_t GetBatteryLevel(uint16_t vbat); //get battery voltage level
//check battery voltage and level then print via USART
bool Battery_CheckVoltage(void);

extern volatile bool bCheckBattery_Flag;    //flag for battery check
//macro for testing if battery level is low
#define IS_BATTERY_LOW() (GetBatteryLevel(GetBatteryVoltage()) == 0)

#endif