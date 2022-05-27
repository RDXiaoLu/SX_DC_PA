#ifndef     __LCD_H_
#define     __LCD_H_

#include "STC8H.h"

//引脚定义
/*******************************************************/
sbit LCDRS=P0^0;     //LCD1602数据/命令选择端口
sbit LCDRW=P0^1;     //LCD1602读写控制端口
sbit LCDEN=P0^2;     //LCD1602使能信号端口
#define LCDDATA  P2  //LCD1602数据端口（仅用DB4至DB7）
/*******************************************************/
#define  LCD_NUM 1

//RELAY外设工作结构体，利用面向对象的思想封装RELAY外设
/*******************************************************/
struct lcd_operations {
	int num;/* num-多少个LED设备 */       
	void (*write) (unsigned char cmdordata,unsigned char writetype); /* 写入液晶模组命令或数据, cmdordata-命令和writetype写数据或写命令 */
	void (*dis_line)(unsigned char *str, int y);
};
//函数全局声明
extern struct lcd_operations LCD_fops;

void LCD_Init(void);

#endif