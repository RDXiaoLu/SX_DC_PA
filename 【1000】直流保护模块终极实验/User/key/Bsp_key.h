#ifndef     __KEY_H_
#define     __KEY_H_

#include "STC8H.h"

//���Ŷ���
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

//LED���蹤���ṹ�壬������������˼���װLED����
/*******************************************************/
struct key_operations {
	int num;/* num-���ٸ�KEY�豸 */       
	int (*read) (int which); /*��ⰴ������ which-�ĸ����� ���ذ���״̬ 0-���� 1-����*/
};

extern struct key_operations KEY_fops;

void KEY_Init(void);

#endif