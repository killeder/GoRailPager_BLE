/*-----------------------------------------------------------------
*@file     Human_Interface.h
*@brief    Human interface and displaying utilities
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/28
-----------------------------------------------------------------------*/
#ifndef HUMAN_INTERFACE_H
#define HUMAN_INTERFACE_H

//Print buildinfo and version info via seiralport
void ShowBuildInfo(void);
//Show decoded LBJ message on LCD
void ShowMessageLBJ(POCSAG_RESULT* POCSAG_Msg);
//Show RSSI level according to rssi value received
void ShowRSSILevel(float rssi);

#endif

