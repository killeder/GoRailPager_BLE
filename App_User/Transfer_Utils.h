/**
  ******************************************************************************
  * @file    Transfer_Uilts.h
  * @author  Xie Yingnan(xieyingnan1994@163.com)
  * @version V1.0.0
  * @date    2021-08-24
  * @brief   This file provides functions of BLE and Serialport transfer
  ******************************************************************************
**/
#ifndef TRANSFER_UTILS_H
#define TRANSFER_UTILS_H

//Print buildinfo and version info via seiralport
void UART_ShowBuildInfo(void);
//parse command received from serial port
void UART_ParseCmd(char* CmdStr);
//Transfer decoded LBJ message on via BLE
void BLE_TransferLBJ(POCSAG_RESULT* POCSAG_Msg,int16_t rssi_10x_int);
//check HC-08 BLE module presence
bool BLE_CheckPresence(void);
//send battery alarm message via BLE
void BLE_SendBatteryAlarm(void);
//parse BLE command string
void BLE_ParseCmd(char* BLE_Cmd);
#endif
