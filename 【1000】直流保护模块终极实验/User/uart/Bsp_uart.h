#ifndef     __UART_H_
#define     __UART_H_

#include "STC8H.h"
#include "stdio.h"
//���Ŷ���
/*******************************************************/

/*******************************************************/
#define UART_NUM 1
#define SYSCLK 24000000UL		//ϵͳʱ��Ƶ��ֵ
#define BAUD_SET  (65536-SYSCLK/9600/4)//�������趨����� 
/*****************************������������***************************/								   
void UART1_Init(void);			//����1��ʼ������


//UART���蹤���ṹ�壬������������˼���װLED����
/*******************************************************/
struct uart_operations {
	int num;/* num-���ٸ�LED�豸 */       
};

extern struct uart_operations UART_fops;

void UART1_Init(void);

#endif