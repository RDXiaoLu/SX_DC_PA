#include "Bsp_uart.h"

static int LED_CTL(int which, int status);
static void U1SEND_C(unsigned char SEND_C);		//串口1发送单字符数据函数
char putchar(char ch);			//发送字符重定向函数

/* C99支持乱序赋值结构体 C89不支持
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//初始化LED外设结构体
struct uart_operations UART_fops = {UART_NUM};

/**
 * @brief  UART1_Init() 串口1初始化
 * @param  无
 * @retval 无
 */
void UART1_Init(void)
{
	SCON=0x50;			//181结构，可变波特率，允许数据接收
	AUXR|=0x01;			//串口1选择定时器2为波特率发生器
	AUXR|=0x04;			//定时器时钟1T模式
	T2L=BAUD_SET;		//设置定时初始值
	T2H=BAUD_SET>>8;//设置定时初始值
	AUXR|=0x10;			//定时器2开始计时
	RI=0;TI=0;			//清除接收数据标志位和发送数据标志位
}

/**
 * @brief  U1SEND_C 串口1发送单字符数据	 有形参SEND_C即为欲发送单字节
 * @param  无
 * @retval 无
 */
void U1SEND_C(unsigned char SEND_C)
{
	TI=0;				//清除发送完成标志位
	SBUF=SEND_C;		//发送数据
	while(!TI);			//等待数据发送完成
}

/**
 * @brief  char putchar(char ch) 发送字符重定向
 * @param  无
 * @retval 无
 */
char putchar(char ch)
{
	U1SEND_C((unsigned char)ch);//通过串口1发送数据
	return ch;
}
