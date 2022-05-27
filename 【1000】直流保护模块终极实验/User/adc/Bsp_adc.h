#ifndef     __ADC_H_
#define     __ADC_H_

#include "STC8H.h"

//引脚定义
/*******************************************************/
//P1^2为输入电压采集引脚
/*******************************************************/
#define ADC_NUM 1

//ADC外设工作结构体，利用面向对象的思想封装LED外设
/*******************************************************/
struct adc_operations {
	int num;/* num-多少个ADC设备 */       
	int (*adc_get)(unsigned char which); /* 控制ADC, 获取ADC采样值 which 0x00 0x02 P1.0或P1.2引脚ADC*/
};

extern struct adc_operations ADC_fops;

void ADC_Init(void);

#endif