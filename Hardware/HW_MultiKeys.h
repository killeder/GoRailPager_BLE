/*********************************************************************************************
文件名：HW_MultiKeys.h - 多重按钮的驱动实现 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年09月01日　　　　  
修改日志：　　
　　NO.1-								
**********************************************************************************************
说明：1.
**********************************************************************************************/
#ifndef HW_MULTIKEYS_H
#define HW_MULTIKEYS_H
//按钮初始化函数
void MultiKeys_Init(void);
//按键处理函数
void KeyProcess(void);
//判断按键的变量
extern uint8_t Key_Trig;	//按键单击触发
extern uint8_t Key_Continue;	//按键hold住
//extern uint8_t EnContinueStrike;	//允许连击标志
//按键键值码
#define CONFIG_KEY		0x01
#define MODE_KEY		0x02
#endif
