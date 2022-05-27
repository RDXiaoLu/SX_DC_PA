#ifndef     __KEY_H_
#define     __KEY_H_

#include "STC8H.h"

//引脚定义
/*******************************************************/
sbit KEY1 = P3^2;
sbit KEY2 = P3^3;
sbit KEY3 = P3^4;
sbit KEY4 = P3^5;
sbit KEY5 = P3^6;
sbit KEY6 = P3^7;
sbit KEY7 = P1^7;
sbit KEY8 = P5^5;
/*******************************************************/
#define KEY_NUM 8

//LED外设工作结构体，利用面向对象的思想封装LED外设
/*******************************************************/
struct key_operations {
	int num;/* num-多少个KEY设备 */       
	int (*read) (int which); /*检测按键引脚 which-哪个按键 返回按键状态 0-按下 1-弹起*/
};

extern struct key_operations KEY_fops;

void KEY_Init(void);

#endif