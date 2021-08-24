/*-----------------------------------------------------------------------
*@file     Radio_Utils.h
*@brief    Radio signal receiving utilities
*@author   Xie Yinanan(xieyingnan1994@163.com)
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#ifndef RADIO_UTILS_H
#define RADIO_UTILS_H

bool Radio_Init(void);//Detecting Radio CC1101 and init
void RF_Rx_Callback(void);	//Callback on CC1101 received a data packet
void RF_RxData_Handler(void);	//Handler of rx data on data arrival
							//Call POCSAG parse of raw data
extern volatile bool bRadioDataArrival;//flag to indicate radio data arrived

#endif
