#ifndef     __LAP_H_
#define     __LAP_H_

#include "STC8H.h"

//���Ŷ���
/*******************************************************/
#define Region_1   0x0000	//����1 �洢Ƿѹ����
#define Region_2   0x0200	//����2 �洢��ѹ����
#define Region_3   0x0400	//����3 �洢Ƿѹ����
#define Region_4   0x0600	//����4 �洢Ƿѹ����
#define Region_5   0x0800	//����5 �洢Ƿѹ����
#define Region_6   0x0A00	//����6 �洢Ƿѹ����
/*******************************************************/
#define IAP_NUM 1

//LED���蹤���ṹ�壬������������˼���װLED����
/*******************************************************/
struct iap_operations {
	int num;/* num-���ٸ�lap�豸 */       
	void (*program)(int addr, char dat);
	char (*read)(int addr); 
	void (*erase)(int addr); 
	void (*int_program)(int addr, int dat);
	int (*int_read)(int addr);
	void (*int_erase)(int addr); 
};

extern struct iap_operations IAP_fops;



#endif