/**
  ******************************************************************************
  * @file    Transfer_Uilts.c
  * @author  Xie Yingnan(xieyingnan1994@163.com)
  * @version V1.0.0
  * @date    2021-08-24
  * @brief   This file provides functions of BLE and Serialport transfer
  ******************************************************************************
**/
#include "GoRailPager_BLE.h"
/**
  * @brief  get RSSI level according to given RSSI value
  * @param	rssi_10x_int: 10 times of RSSI value, trimmed to integer
  * @retval RSSI level, ranged from 1 to 5, level 5 is the highest
  */
static uint8_t Get_RSSI_Level(int32_t rssi_10x_int)
{
  const int32_t notchs[] = {-1050, -1000, -950, -900, -850};
  uint8_t level = 0;
  for (uint8_t idx = 0; idx < 5; idx++)
  {
    if (rssi_10x_int >= notchs[idx])
      level++;
  }
  return level;
}
/**
  * @brief  copy fix length of string as well as skim <sp>
  * @param	dst: buffer to hold result string
  * @param	src: buffer to hold source string
  * @param	len_limit: only copy heading 'len_limit' characters of source 
  * @retval none
  */
static void TextSkimSpace(char *dst, char *src, uint8_t len_limit)
{
  while ((*src != '\0') && (len_limit--))
  {
    *dst = *src;//do copy and move after source pointer
    src++;
    //only increase destination pointer if current character not <sp>
    if (*dst != '\x20')
      dst++;  
  }
  *dst = '\0';//fill tail with '\0' for string termination
}
/**
  * @brief  check if module responsed command correctly in given timeout
  * @param  Cmd: AT command to send
  * @param  WaitFor: response string to wait
  * @param  Timeout: time out for waiting module response
  * @retval true:module responsed correctly in given timeout
  */
static bool BLE_WaitCommandAck(char* AT_Cmd, const char* WaitFor, uint16_t Timeout)
{
  uint16_t cnt = 0;//counter for timeout
  //clear DMA rx buffer flags and tx counter, must be done BEFORE DMA Tx
  HW_USART_DMA_RxClear();
  //send AT command first
  HW_USART_DMA_SendBuffer((uint8_t*)AT_Cmd,strlen(AT_Cmd));
  //check response
  do{
    if(bUSART_DMA_RxFinish)
    {
      if(strstr(USART_DMA_RxBuffer,WaitFor) != NULL)
      {
        HW_USART_DMA_RxClear();
        return true;
      }
    }
    Delay_ms(1);
  }while(++cnt < Timeout);
  return false;
}
/**
  * @brief  Show BLE name
  * @param  none
  * @retval None
  */
static void BLE_ShowName(void)
{
  uint16_t cnt = 0;
  char AT_Cmd[] = "AT+NAME=?";

  HW_USART_DMA_RxClear();
  HW_USART_DMA_SendBuffer((uint8_t*)AT_Cmd,strlen(AT_Cmd));
  do{
    if(bUSART_DMA_RxFinish)
    {
      MSG("BLE name:%s\r\n",USART_DMA_RxBuffer);
      HW_USART_DMA_RxClear();
      return;
    }
    Delay_ms(1);
  }while(++cnt < 200);
  MSG("Fail!\r\n");
}
/**
  * @brief  Set BLE name
  * @param  name: pointed to name string
  * @retval true:OK false:failed
  */
static bool BLE_SetName(char* name)
{
  char AT_Cmd[32] = {0};
  bool result;

  sprintf(AT_Cmd,"AT+NAME=%s",name);
  result = BLE_WaitCommandAck(AT_Cmd,"OK",200);
  return result;
}
/**
  * @brief  show serial port command help
  * @param  none
  * @retval None
  */
static void UART_ShowCmdHelp(void)
{
  MSG("Commands list:\r\n");
  MSG("?\tShow command help\r\n");
  MSG("$V\tShow firmware version\r\n");
  MSG("$BATT\tShow battery voltage in mV and level\r\n");
  MSG("$NAME\tShow BLE name\r\n");  
  MSG("$NAME=xxx\tSet BLE name to \"xxx\"\r\n");
}
/**
  * @brief  Print build info and version info via seiralport
  * @param  none
  * @retval None
  */
void UART_ShowBuildInfo(void)
{
  MSG("%s Version%s (Build Date %s %s)\r\n", APP_NAME_STR,
      VERTION_STR, BUILD_DATE_STR, BUILD_TIME_STR); //串口打印版本信息
  MSG("Xie Yingnan Works.<xieyingnan1994@163.com>\r\n");
}
/**
  * @brief  parse command received from serial port
  * @param  CmdStr:pointer to command string
  * @retval None
  */
void UART_ParseCmd(char* CmdStr)
{
  if(CmdStr[0] == '$')
  {
    if(!strncmp(CmdStr+1,"V",1))
    {
      UART_ShowBuildInfo();
    }
    else if(!strncmp(CmdStr+1,"BATT",4))
    {
      Battery_CheckVoltage();
    }
    else if(!strncmp(CmdStr+1,"NAME",4))
    {
      char* pos;
      if((pos=strchr(CmdStr,'=')) != NULL)
      {
        if(BLE_SetName(pos+1))
          MSG("OK!");
        else
          MSG("Fail!");
      }
      else
        BLE_ShowName();
    }
    else
    {
      MSG("Unsupported command.");
    }
  }
  else if(CmdStr[0] == '?')
  {
    UART_ShowCmdHelp();
  }
  else
  {
    MSG("Bad command, type '?' for help.");
  }
  MSG("\r\n");
}
/**
  * @brief  Transfer LBJ message and rssi via BLE interface
  * @param  POCSAG_Msg: pointer to decoded POCSAG message
  * @param	rssi_10x_int: 10 times of RSSI value, trimmed to integer
  * @retval None
  */
void BLE_TransferLBJ(POCSAG_RESULT *POCSAG_Msg, int16_t rssi_10x_int)
{
	/* string "POCSAG_Msg->txtMsg" normally holds 15 characters. i.e.
		"47963<sp>100<sp>23456". Symbol <sp> signifies space, aka.'\0x20'.
		the "-" character could also appear in above string, sinifying 
		non-available of the corresponding infomation.*/
  char LBJ_Info[3][8] = {0};
  char BLE_DataGram[48] = {0};
  char dir_ch = 'X';
  //split traincode/speed/milemark strings and skim space
  for(uint8_t i=0;i<3;i++)
  {
    TextSkimSpace(LBJ_Info[i],POCSAG_Msg->txtMsg+i*5,5);
  }
  //insert '.' in milemark string
  uint8_t position = strlen(LBJ_Info[2]);
  LBJ_Info[2][position] = LBJ_Info[2][position-1];
  LBJ_Info[2][position-1] = '.';
  LBJ_Info[2][position+1] = '\0';
  //set dir_ch to 'X' or 'S' according to POCSAG function code
  if(POCSAG_Msg->FuncCode == FUNC_SHANGXING)
    dir_ch = 'S';
  else if(POCSAG_Msg->FuncCode == FUNC_XIAXING)
    dir_ch = 'X';
  //make up LBJ message BLE datagram
  //e.g. $X,50229,104,1168.0,-105.0,2$
  sprintf(BLE_DataGram,"$%c,%s,%s,%s,%hd.%hd,%hu$",
          dir_ch,LBJ_Info[0],LBJ_Info[1],LBJ_Info[2],
          rssi_10x_int/10,(0-rssi_10x_int)%10,
          Get_RSSI_Level(rssi_10x_int));        
  //transmit datagram via USART DMA
  HW_USART_DMA_SendBuffer((uint8_t*)BLE_DataGram,
                          strlen(BLE_DataGram));
}
/**
  * @brief  check presence of BLE module HC-08
  * @param  none
  * @retval true:module response correctly
  */
bool BLE_CheckPresence(void)
{
  uint8_t i = 0;
  bool IsFound = false;

  MSG("Check BLE module presence:");
  while((i < 5)&&(IsFound == false))
  {
    //check if BLE module response "OK" to "AT" command in 300ms
    if(BLE_WaitCommandAck("AT","OK",300))
      IsFound = true;
    else
    {
      Delay_ms(50);//if no response, delay 50ms and try again
      i++;
    }
  }

  if(IsFound)
    MSG("OK");
  else
    MSG("Fail");
  MSG("!\r\n");
  return IsFound;
}
/**
  * @brief  send battery alarm message via BLE
  * @param  none
  * @retval none
  */
void BLE_SendBatteryAlarm(void)
{
  uint16_t vbat  = GetBatteryVoltage();
  char BLE_Msg[32] = {0};

  sprintf(BLE_Msg,"#BATT_ALM:%hu#",vbat);
  HW_USART_DMA_SendBuffer((uint8_t*)BLE_Msg,
                          strlen(BLE_Msg)); 
}
/**
  * @brief  send battery voltage and level via BLE
  * @param  none
  * @retval none
  */
void BLE_SendBatteryVoltage(void)
{
  uint16_t vbat  = GetBatteryVoltage();
  uint8_t level = GetBatteryLevel(vbat);
  char BLE_Msg[32] = {0};

  sprintf(BLE_Msg,"#VBAT:%hu,%hu#",vbat,level);
  HW_USART_DMA_SendBuffer((uint8_t*)BLE_Msg,
                          strlen(BLE_Msg)); 
}
/**
  * @brief  Parse BLE command strobe
  * @param  BLE_Cmd: pointed to command string
  * @retval none
  */
void BLE_ParseCmd(char* BLE_Cmd)
{
  if(strstr(BLE_Cmd,"#VBAT_CHK#") != NULL)
    BLE_SendBatteryVoltage();
}