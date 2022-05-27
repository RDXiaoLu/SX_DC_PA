#ifndef     __UART_H_
#define     __UART_H_

#include "STC8H.h"
#include "stdio.h"
//引脚定义
/*******************************************************/

/*******************************************************/
#define UART_NUM 1
#define SYSCLK 24000000UL		//系统时钟频率值
#define BAUD_SET  (65536-SYSCLK/9600/4)//波特率设定与计算 
/*****************************函数声明区域***************************/								   
void UART1_Init(void);			//串口1初始化函数


//UART外设工作结构体，利用面向对象的思想封装LED外设
/*******************************************************/
struct uart_operations {
	int num;/* num-多少个LED设备 */       
};

extern struct uart_operations UART_fops;

void UART1_Init(void);

#endif