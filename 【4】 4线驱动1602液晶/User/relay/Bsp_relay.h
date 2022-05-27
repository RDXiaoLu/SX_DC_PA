#ifndef     __RELAY_H_
#define     __RELAY_H_

#include "STC8H.h"

//���Ŷ���
/*******************************************************/
sbit relay = P0^3;
/*******************************************************/
#define RELAY_NUM 1

//RELAY���蹤���ṹ�壬������������˼���װRELAY����
/*******************************************************/
struct relay_operations {
	int num;/* num-���ٸ�LED�豸 */       
	int (*relay_ctl) (int which, int status); /* ����RELAY, which:1-��һ��RELAY,2-�ڶ���RELAY..., status:1-��,0-�� */
};

extern struct relay_operations RELAY_fops;

void RELAY_Init(void);

#endif