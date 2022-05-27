#ifndef     __LED_H_
#define     __LED_H_

#include "STC8H.h"

//���Ŷ���
/*******************************************************/
sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit LED3 = P2^2;
sbit LED4 = P2^3;
/*******************************************************/
#define LED_NUM 4

//LED���蹤���ṹ�壬������������˼���װLED����
/*******************************************************/
struct led_operations {
	int num;/* num-���ٸ�LED�豸 */       
	int (*led_ctl) (int which, int status); /* ����LED, which:1-��һ��LED,2-�ڶ���LED..., status:1-��,0-�� */
};

extern struct led_operations LED_fops;

void LED_Init(void);

#endif