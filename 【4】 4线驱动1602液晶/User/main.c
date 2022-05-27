/*********************************************************************
 * ʵ�����ƣ�	��ˮ��ʵ��
 * ѡ���鼮��	�ϲ�-������ǳ��STC8��ǿ��51��Ƭ�����׹��ԡ�2022�ѳ���
							�²�-������ǳ��STC8��ǿ��51��Ƭ��ʵս���ԡ�����д��
 * �鼮���ߣ�	��˳����� �廪��ѧ���������
 * �Ա����̣�	https://520mcu.taobao.com/
 * ʵ��ƽ̨��	˼�޵���STC8��ս����ϵ��С���ӿ�����
 * оƬ�ͺţ�	STC8H3K64S4-LQFP32������΢�������ֲ��STC����ϵ�е�Ƭ����
 * ʱ��˵����	Ƭ��IRC����24MHzʱ�ӣ�ʹ��STC-ISP���س���ʱ����
 * ʵ��˵����	��
*********************************************************************/
#include "STC8H.h"
#include "./led/Bsp_led.h"
#include "./key/Bsp_key.h"
#include "./lcd/Bsp_lcd.h"
/*****************************������������***************************/
static void delay(unsigned int Count); /* ������ʱ */
static void OS_Init(void);	 /* ϵͳӲ����ʼ�� */

unsigned char table1[17]="==SYS PASSWORD==";//LCD1602��ʾ�ַ�������1
unsigned char table2[17]="****************";//LCD1602��ʾ�ַ�������2


void main()
{
	
	
	/* ϵͳӲ����ʼ�� */
	OS_Init();
	delay(1000);
	LCD_fops.write(table1[0],1);//д��table1[]����
	LCD_fops.write(table1[1],1);//д��table2[]����
	LCD_fops.write(table1[2],1);//д��table2[]����
	LCD_fops.write(table1[3],1);//д��table1[]����
	LCD_fops.write(table1[4],1);//д��table2[]����
	LCD_fops.write(table1[5],1);//д��table2[]����
	LCD_fops.write(table1[6],1);//д��table1[]����
	LCD_fops.write(table1[7],1);//д��table2[]����
	LCD_fops.write(table1[8],1);//д��table2[]����
	LCD_fops.write(table1[9],1);//д��table2[]����
	LCD_fops.write(table1[10],1);//д��table2[]����
	LCD_fops.write(table1[11],1);//д��table1[]����
	LCD_fops.write(table1[12],1);//д��table2[]����
	LCD_fops.write(table1[13],1);//д��table2[]����
	LCD_fops.write(table1[14],1);//д��table1[]����
	LCD_fops.write(table1[15],1);//д��table2[]����
	LCD_fops.write(table1[16],1);//д��table2[]����
	LCD_fops.dis_line(table2,1);
	while(1);	
}

/**
 * @brief  OS_Init ϵͳӲ����ʼ��
 * @param  ��
 * @retval ��
 */
static void OS_Init(void)
{
	LED_Init();	
	KEY_Init();
	LCD_Init();
}

/**
 * @brief  delay ������ʱ
 * @param  ��
 * @retval ��
 */
static void delay(unsigned int Count)
{
  unsigned char i,j;
  while (Count--)//Count�βο�����ʱ����
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}


