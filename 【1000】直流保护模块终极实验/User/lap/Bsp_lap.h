#ifndef     __LAP_H_
#define     __LAP_H_

#include "STC8H.h"

//引脚定义
/*******************************************************/
#define Region_1   0x0000	//扇区1 存储欠压参数
#define Region_2   0x0200	//扇区2 存储过压参数
#define Region_3   0x0400	//扇区3 存储欠压参数
#define Region_4   0x0600	//扇区4 存储欠压参数
#define Region_5   0x0800	//扇区5 存储欠压参数
#define Region_6   0x0A00	//扇区6 存储欠压参数
/*******************************************************/
#define IAP_NUM 1

//LED外设工作结构体，利用面向对象的思想封装LED外设
/*******************************************************/
struct iap_operations {
	int num;/* num-多少个lap设备 */       
	void (*program)(int addr, char dat);
	char (*read)(int addr); 
	void (*erase)(int addr); 
	void (*int_program)(int addr, int dat);
	int (*int_read)(int addr);
	void (*int_erase)(int addr); 
};

extern struct iap_operations IAP_fops;



#endif