#ifndef     __LED_H_
#define     __LED_H_

#include "STC8H.h"

//引脚定义
/*******************************************************/
sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit LED3 = P2^2;
sbit LED4 = P2^3;
/*******************************************************/
#define LED_NUM 4

//LED外设工作结构体，利用面向对象的思想封装LED外设
/*******************************************************/
struct led_operations {
	int num;/* num-多少个LED设备 */       
	int (*led_ctl) (int which, int status); /* 控制LED, which:1-第一个LED,2-第二个LED..., status:1-亮,0-灭 */
};

extern struct led_operations LED_fops;

void LED_Init(void);

#endif