#ifndef     __ADC_H_
#define     __ADC_H_

#include "STC8H.h"

//���Ŷ���
/*******************************************************/
//P1^2Ϊ�����ѹ�ɼ�����
/*******************************************************/
#define ADC_NUM 1

//ADC���蹤���ṹ�壬������������˼���װLED����
/*******************************************************/
struct adc_operations {
	int num;/* num-���ٸ�ADC�豸 */       
	int (*adc_get)(unsigned char which); /* ����ADC, ��ȡADC����ֵ which 0x00 0x02 P1.0��P1.2����ADC*/
};

extern struct adc_operations ADC_fops;

void ADC_Init(void);

#endif