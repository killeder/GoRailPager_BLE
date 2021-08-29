/**
  ******************************************************************************
  * @file    Battery_Utils.c
  * @author  Xie Yingnan(xieyingnan1994@163.com)
  * @version V1.0.0
  * @date    2021-08-23
  * @brief   Utilities for battery voltage and level check
  ******************************************************************************
**/
#include "GoRailPager_BLE.h"
//define battery level "notch" in mV
const uint16_t Batt_Levels[] = {3650U,3720U,3800U,3900U,4000U};
volatile bool bCheckBattery_Flag = false;    //flag for battery check
/**
  * @brief  get battery voltage in mV, e.g. 3700 signifies 3700mV
  * @param  none
  * @note   the result is average value of 5 times measurements
  * @retval battery voltage in mV
  */
uint16_t GetBatteryVoltage(void)
{
    uint32_t ave_vbat = 0UL;

    for(uint8_t idx = 0; idx < 5;idx++)
    {
        ave_vbat += Get_AIN_Voltage()*2;
        //AIN_Voltage is 1/2 of Battery voltage
    }
    ave_vbat /= 5;//calculate average value of 5 measurements
    //ave_vbat is in accuracy of 0.1mV
    //devided by 10 to return voltage value in accuracy of 1mV 
    return((uint16_t)(ave_vbat/10));
}
/**
  * @brief  get battery voltage level according to given voltage
  * @param  vbat: battery voltage in mV
  * @note   the "notch" of levels is defined in global array "Batt_Levels"
  * @retval battery level in range 1 to 5, return value <1 signifies low battery 
  */
uint8_t GetBatteryLevel(uint16_t vbat)
{
    uint8_t level = 0U;
    for(uint8_t idx = 0;idx < 5;idx++)
    {
        if(vbat >= Batt_Levels[idx])
            level++;
    }
    return level;//return 0 signifies low battery level
}
/**
  * @brief  check battery voltage and level then print via USART 
  * @param  none
  * @retval true if voltage level is OK
  */
bool Battery_CheckVoltage(void)
{
    uint16_t vbat  = GetBatteryVoltage();
    uint8_t level = GetBatteryLevel(vbat);

    MSG("Battery check:");
    if(level > 0)
    {
        MSG("%umV,Level%u,OK!\r\n",vbat,level);
        return true;
    }
    else
    {
        MSG("%umV,LOW!\r\n",vbat);
        return false;
    }
}
