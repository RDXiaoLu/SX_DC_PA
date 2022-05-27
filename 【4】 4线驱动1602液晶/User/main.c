/*********************************************************************
 * 实验名称：	流水灯实验
 * 选配书籍：	上册-《深入浅出STC8增强型51单片机进阶攻略》2022已出版
							下册-《深入浅出STC8增强型51单片机实战攻略》还在写作
 * 书籍作者：	龙顺宇编著 清华大学出版社出版
 * 淘宝店铺：	https://520mcu.taobao.com/
 * 实验平台：	思修电子STC8“战将”系列小王子开发板
 * 芯片型号：	STC8H3K64S4-LQFP32（程序微调后可移植至STC其他系列单片机）
 * 时钟说明：	片内IRC高速24MHz时钟，使用STC-ISP下载程序时配置
 * 实验说明：	无
*********************************************************************/
#include "STC8H.h"
#include "./led/Bsp_led.h"
#include "./key/Bsp_key.h"
#include "./lcd/Bsp_lcd.h"
/*****************************函数声明区域***************************/
static void delay(unsigned int Count); /* 粗略延时 */
static void OS_Init(void);	 /* 系统硬件初始化 */

unsigned char table1[17]="==SYS PASSWORD==";//LCD1602显示字符串数组1
unsigned char table2[17]="****************";//LCD1602显示字符串数组2


void main()
{
	
	
	/* 系统硬件初始化 */
	OS_Init();
	delay(1000);
	LCD_fops.write(table1[0],1);//写入table1[]内容
	LCD_fops.write(table1[1],1);//写入table2[]内容
	LCD_fops.write(table1[2],1);//写入table2[]内容
	LCD_fops.write(table1[3],1);//写入table1[]内容
	LCD_fops.write(table1[4],1);//写入table2[]内容
	LCD_fops.write(table1[5],1);//写入table2[]内容
	LCD_fops.write(table1[6],1);//写入table1[]内容
	LCD_fops.write(table1[7],1);//写入table2[]内容
	LCD_fops.write(table1[8],1);//写入table2[]内容
	LCD_fops.write(table1[9],1);//写入table2[]内容
	LCD_fops.write(table1[10],1);//写入table2[]内容
	LCD_fops.write(table1[11],1);//写入table1[]内容
	LCD_fops.write(table1[12],1);//写入table2[]内容
	LCD_fops.write(table1[13],1);//写入table2[]内容
	LCD_fops.write(table1[14],1);//写入table1[]内容
	LCD_fops.write(table1[15],1);//写入table2[]内容
	LCD_fops.write(table1[16],1);//写入table2[]内容
	LCD_fops.dis_line(table2,1);
	while(1);	
}

/**
 * @brief  OS_Init 系统硬件初始化
 * @param  无
 * @retval 无
 */
static void OS_Init(void)
{
	LED_Init();	
	KEY_Init();
	LCD_Init();
}

/**
 * @brief  delay 粗略延时
 * @param  无
 * @retval 无
 */
static void delay(unsigned int Count)
{
  unsigned char i,j;
  while (Count--)//Count形参控制延时次数
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}


