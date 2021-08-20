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
//Show splash screen and version info on LCD
void ShowSplashScreen(void);
//Show RSSI level according to rssi value received
void ShowRSSILevel(float rssi);

//screen type
typedef enum{SCREEN_NORMAL = 0,SCREEN_FAULT,SCREEN_APPNAME,SCREEN_AUTHOR} SCREENTYPE;
//show screen on led displayer according to type
void LED_SwitchScreen(SCREENTYPE type);

extern volatile bool bEnableMessageBeep;

#endif

