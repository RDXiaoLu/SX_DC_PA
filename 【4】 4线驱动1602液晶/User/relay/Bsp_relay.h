#ifndef     __RELAY_H_
#define     __RELAY_H_

#include "STC8H.h"

//引脚定义
/*******************************************************/
sbit relay = P0^3;
/*******************************************************/
#define RELAY_NUM 1

//RELAY外设工作结构体，利用面向对象的思想封装RELAY外设
/*******************************************************/
struct relay_operations {
	int num;/* num-多少个LED设备 */       
	int (*relay_ctl) (int which, int status); /* 控制RELAY, which:1-第一个RELAY,2-第二个RELAY..., status:1-开,0-关 */
};

extern struct relay_operations RELAY_fops;

void RELAY_Init(void);

#endif