/*-----------------------------------------------------------------------
*@file     RF_Radio_Utility.c
*@brief    Radio signal receiving utilities
*@author   Xie Yinanan(xieyingnan1994@163.com)
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#include "GoRailPager_7Seg.h"

volatile bool bRadioDataArrival = false;//flag to indicate radio data arrived
/*-----------------------------------------------------------------------
*@brief		Detecting Radio CC1101 and init
*@param		none
*@retval	1 - CC1101 init ok; 0 - CC1101 init failed
-----------------------------------------------------------------------*/
bool Radio_Init(void)
{
	MSG("CC1101 Initializing...\r\n");
	//Frequency:821.2375MHz
	//1200bps,2FSK,Freq.Dev:4.5khz,RxBW:58.0kHz,Preamble:16Bytes
	//Fixed packet size 16 bytes, Not allow synccode bit error,
	//turn on carrier detecting, turn off CRC filtering.
	//Sync code is 0xEA27(reversed low 16-bit sync code of 
	//standard POCSAG)
	int8_t cc1101_state = CC1101_Setup(821.2375f,1.2f,4.5f,58.0f,0,16);
	MSG("CC1101 initialize ");
	if(cc1101_state == RADIO_ERR_NONE)	//if setup is OK
	{
		MSG("OK!\r\n");
		return true;
	}
	else	//if setup encountered an error
	{
		MSG("failed! StateCode:%d\r\n",cc1101_state);
		MSG("System halt!\r\n");
		return false;
	}
}

/*-----------------------------------------------------------------------
*@brief		Callback on CC1101 received a data packet
*@param		According the setting during CC1101 setup, CC1101 turned into
*           IDLE state afer received packet, raw data is saved in rx FIFO
*@retval	None
-----------------------------------------------------------------------*/
void Rf_Rx_Callback(void)
{
	if(!bRadioDataArrival)
		bRadioDataArrival = true;//Set data arrival flag
}
/*-----------------------------------------------------------------------
*@brief		Read RxFIFO of CC1101 and process the raw data
*@detail	According the setting during CC1101 setup, CC1101 turned into
*           IDLE state afer received packet, raw data is saved in rx FIFO.
*           CC1101 will hold in IDLE state unless we send "start rx" comm-
*           and again.
*@param 	none
*@retval	none
-----------------------------------------------------------------------*/
void RxData_Handler(void)
{
	uint8_t* batch_buff = NULL;	//buffer for storge raw codeword data
	uint32_t batch_len = CC1101_GetPacketLength(false);
			//get packet length,it was set to 16 bytes when CC1101 init.
	uint32_t actual_len;//actual length of raw data, identical to batch_len
						//when CC1101 was set to fix-length packet mode
	POCSAG_RESULT PocsagMsg;//structure for storge POCSAG parse result

	if((batch_buff=(uint8_t*)malloc(batch_len*sizeof(uint8_t))) != NULL)
	{
		memset(batch_buff,0,batch_len);	//clear codeword buffer
		CC1101_ReadDataFIFO(batch_buff,&actual_len);//read raw data from FIFO
		float rssi = CC1101_GetRSSI();//for CC1101 turned to IDLE mode afer rx
		uint8_t lqi = CC1101_GetLQI();//RSSI and LQI here is in correspondence
									  //with current batch of raw data

		MSG("\r\n[LBJ Message Arrival]\r\n");
		MSG("Received %u bytes of raw data.\r\n",actual_len);
		MSG("RSSI:%d.%d\r\n",(int32_t)rssi, ((int32_t)(rssi*10))*(-1)%10);
		MSG("LQI:%u\r\n",lqi);
		MSG("Raw data:\r\n");
		for(uint32_t i=0;i < actual_len;i++)
		{
			MSG("%02Xh ",batch_buff[i]);//print raw data
			if((i+1)%16 == 0)
				MSG("\r\n");	//16 byte per line
		}
		//Parse LBJ info
		int8_t state = POCSAG_ParseCodeWordsLBJ(&PocsagMsg,batch_buff,
												 actual_len,true);							     		 
		if(state == POCSAG_ERR_NONE)
		{										
			MSG("Address:%u,Function:%d.\r\n",PocsagMsg.Address,PocsagMsg.FuncCode);
												//show address code and function code
			MSG("LBJ Message:%s.\r\n",PocsagMsg.txtMsg);//show decoded text message
			if(PocsagMsg.Address == LBJ_MESSAGE_ADDR)
			{
				ShowMessageLBJ(&PocsagMsg);	//show LBJ message on 7-seg LED
				ShowRSSILevel(rssi);	//show rssi level on 7-seg LED
			}
		}
		else
		{
			MSG("POCSAG parse failed! Errorcode:%d\r\n",state);
		}
		free(batch_buff);
	}
	CC1101_StartReceive(Rf_Rx_Callback);	//re-enable rx, wait for data arrival
}
