/*********************************************************************************************
文件名：delay.c - 公用延时程序　 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年08月31日　　　　  
修改日志：　　
　　NO.1- 2020-08-28 调整参数适配Cortex-M0							
**********************************************************************************************
说明：1.公用微秒级和毫秒级延时程序，使用Systick系统定时器实现。
**********************************************************************************************/
#include "stm32f0xx.h"
#include "delay.h"
/*---------------本模块私有的宏、类型、函数声明、变量----------------------*/
uint32_t  fac_us = 0UL;							//us延时倍乘数
uint32_t fac_ms = 0UL;							//ms延时倍乘数
/*---------------------------------------------------------------------------
函数名：Delay_Init() - 初始化延迟函数
功能：初始化延迟函数
输入参数：无
返回值：无
---------------------------------------------------------------------------*/
void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//选择Systick时钟为HCLK
	fac_us = 48UL;	//HCLK=48M时，Systick计数频率为48MHz，48个tick为1us
	fac_ms = fac_us*1000UL;		//非OS下,代表每个ms需要的systick时钟数
}
/*---------------------------------------------------------------------------
函数名：delay_us - 微秒延迟函数
功能：微秒级延迟
输入参数：uint32_t nus - 要延迟的微妙数
返回值：无
说明：注意延迟最大值在48M时钟条件下为0xffffff/48 = 349525 us
---------------------------------------------------------------------------*/		    								   
void Delay_us(uint32_t nus)
{		
	volatile uint32_t temp;
	SysTick->LOAD = nus*fac_us; 					//时间加载
	SysTick->VAL = 0x00;        					//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//开始倒数
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0X00;      					 //清空计数器	 
}
/*---------------------------------------------------------------------------
函数名：delay_ms - 毫秒延迟函数
功能：微秒级延迟
输入参数：uint16_t nms - 要延迟的毫秒数
返回值：无
说明：注意延迟最大值为0xffffff/(48*1000) = 349 ms
---------------------------------------------------------------------------*/	
void Delay_ms(uint32_t nms)
{	 		  	  
	volatile uint32_t temp;
	SysTick->LOAD = nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;							//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;	//开始倒数
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0X00;       					//清空计数器
}
