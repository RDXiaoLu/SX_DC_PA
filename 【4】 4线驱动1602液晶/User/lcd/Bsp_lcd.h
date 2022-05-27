#ifndef     __LCD_H_
#define     __LCD_H_

#include "STC8H.h"

//���Ŷ���
/*******************************************************/
sbit LCDRS=P0^0;     //LCD1602����/����ѡ��˿�
sbit LCDRW=P0^1;     //LCD1602��д���ƶ˿�
sbit LCDEN=P0^2;     //LCD1602ʹ���źŶ˿�
#define LCDDATA  P2  //LCD1602���ݶ˿ڣ�����DB4��DB7��
/*******************************************************/
#define  LCD_NUM 1

//RELAY���蹤���ṹ�壬������������˼���װRELAY����
/*******************************************************/
struct lcd_operations {
	int num;/* num-���ٸ�LED�豸 */       
	void (*write) (unsigned char cmdordata,unsigned char writetype); /* д��Һ��ģ�����������, cmdordata-�����writetypeд���ݻ�д���� */
	void (*dis_line)(unsigned char *str, int y);
};
//����ȫ������
extern struct lcd_operations LCD_fops;

void LCD_Init(void);

#endif