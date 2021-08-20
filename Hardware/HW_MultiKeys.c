/*********************************************************************************************
文件名：HW_MultiKeys.c - 多重按钮的驱动实现 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年09月01日　　　　  
修改日志：　　
　　NO.1-								
**********************************************************************************************
说明：1.
**********************************************************************************************/
#include "Hardware.h"
/*---------------本模块私有的宏、类型、函数声明、变量----------------------*/
//定义按钮连接的GPIO端口，修改下列宏即可改变接入引脚
#define CONFIG_GPIO_PORT		GPIOA
#define CONFIG_GPIO_CLOCK		RCC_AHBPeriph_GPIOA
#define CONFIG_GPIO_PIN			GPIO_Pin_0	

#define MODE_GPIO_PORT		GPIOA
#define MODE_GPIO_CLOCK		RCC_AHBPeriph_GPIOA
#define MODE_GPIO_PIN		GPIO_Pin_1

/*-----------------------与其他模块共享的变量----------------------------*/
uint8_t Key_Trig = 0;	//代表按键触发(1到0的跳变)
					//仅在按下后第一个扫描周期有值，其他时刻为0，代表按键单击
uint8_t Key_Continue = 0;
					//代表按键连续按下
					//只要按键按下未松开，其值保持不变，一旦松开即回0
//uint8_t EnContinueStrike = 0;	//允许连击标志，控制连击时数据改变的速度
					//在每次执行连击功能后要把此变量设置为0
/*---------------------------------------------------------------------------
函数名：GetKeyCode()
功能：读取按键并合并按键码
输入参数：无
返回值：uint8_t 按键码
---------------------------------------------------------------------------*/
uint8_t GetKeyCode(void)
{
	uint8_t keycode = 0;
	if(CONFIG_GPIO_PORT->IDR & CONFIG_GPIO_PIN)
		keycode |= CONFIG_KEY;
	if(MODE_GPIO_PORT->IDR & MODE_GPIO_PIN)
		keycode |= MODE_KEY;
	return keycode;
}
/*---------------------------------------------------------------------------
函数名：MultiKeys_Init()
功能：按钮初始化
输入参数：无
返回值：无
---------------------------------------------------------------------------*/
void MultiKeys_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(CONFIG_GPIO_CLOCK|MODE_GPIO_CLOCK,ENABLE);//打开时钟
	
	GPIO_InitStructure.GPIO_Pin = CONFIG_GPIO_PIN;//选择CONFIG_KEY引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//设定速率2MHz低速
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(CONFIG_GPIO_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MODE_GPIO_PIN;//选择MODE_KEY引脚
	GPIO_Init(MODE_GPIO_PORT,&GPIO_InitStructure);
}
/*----------------------------------------------------------------------------
函数名：KeyProcess()
参  数：无
返回值：无
功 能：	按键检测和处理
备  注：1.没有按键时ReadData=0,Key_Trig=0,Key_Continue=0;
		2.第一次按下时ReadData=键值,Key_Trig=键值,Key_Continue=键值;
		3.按住不放时ReadData=键值,Key_Trig=0,Key_Continue=键值;
		4.按键松开时ReadData=0,Key_Trig=0,Key_Continue=0;
---------------------------------------------------------------------------*/
void KeyProcess(void)
{
	uint8_t ReadData;		//暂存当前读到的数据
	//static uint8_t Keytime_Count;	//未处于连击状态时长按Keytime_Count个扫描周期，触发连击
									//处于连击状态时控制数值改变的速度
	//static uint8_t InContinueStrike = 0;
									//标志当前是否在连击状态中
	ReadData = GetKeyCode();		//读取按键码(宏展开GPIO_ReadInputDataBit)
	Key_Trig = ReadData&(ReadData^Key_Continue);
	Key_Continue = ReadData;
//----------------单击处理--------------------------
	if(Key_Trig != 0)	 //如果检测到有按键单击
	{
		BeeperMode = BEEP_ONCE;	//蜂鸣器响一声
	}
//----------------连击处理--------------------------
	// if(Key_Continue != 0)	 //如果检测到按键按住不放
	// {
	// 	if(InContinueStrike == 0)	 //如果不在连击状态中
	// 	{
	// 		if(++Keytime_Count > 120)	//如果按住不放超过一定时间就开始连击
	// 		{
	// 			Keytime_Count = 0;	//清零计数变量，准备为接下来控制数据改变速度而计数
	// 			InContinueStrike = 1;	 //切换当前状态为连击状态
	// 			BUZZER_OFF();	//关闭蜂鸣器		
	// 		}
	// 	}
	// 	else					 //如果处于连击状态中
	// 	{
	// 		if((++Keytime_Count > 10)&&(EnContinueStrike == 0))//控制连击时数据改变的速度
	// 		{
	// 			Keytime_Count = 0; //清零计数变量
	// 			EnContinueStrike = 1;	//使能连击
	// 		}			
	// 	}	
	// }
//----------------抬键处理--------------------------
	// if((Key_Continue == 0)&&(Key_Trig == 0))	//如Key_Trig,Key_Continue都为0，说明按键已松开
	// {
	// 	InContinueStrike = 0;	 //退出连击状态
	// 	EnContinueStrike = 0;	 //取消连击是能
	// }																								
}
