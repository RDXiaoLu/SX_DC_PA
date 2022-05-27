#include "Bsp_adc.h"

extern void delay(unsigned int Count); /* 粗略延时 */

static int ADC_GET(unsigned char which);

/* C99支持乱序赋值结构体
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//初始化LED外设结构体
struct adc_operations ADC_fops = {ADC_NUM , ADC_GET};

/**
 * @brief  LED_CTL LED控制
 * @param  which 1-4  D1 D2 D3 D4	 status 1-亮 0-灭
 * @retval 0 设置成功1 设置失败
 */
static int ADC_GET(unsigned char which)
{
	int AD_Vtemp = 0;
	ADC_CONTR &= 0xf0;
	ADC_CONTR |= which;
	ADC_CONTR|=0x40;					//启动ADC转换
	delay(10);								//延时等待转换完毕
	while(!(ADC_CONTR&0x20));	//等待ADC转换结束
	ADC_CONTR &= 0xDF;					//清零ADC转换结束标志位
	AD_Vtemp = (unsigned int)ADC_RES<<4;
	AD_Vtemp |= (unsigned int)ADC_RESL>>4;//处理左对齐格式下的ADC结果
	return AD_Vtemp;
}

/**
 * @brief  ADC_Init ADC初始化
 * @param  无
 * @retval 无
 */
void ADC_Init(void)
{
	ADC_CONTR=0x02;	//选定P1.2引脚作为ADC模拟电压输入引脚
	//配置P1.2为高阻输入模式
	P1M0&=0xFB;			//P0M0.1=0
	P1M1|=0x04;			//P0M1.1=1
	ADCCFG=0x0F;		//转换结果左对齐且ADC转换时钟为SYSCLK/2/16
	ADC_CONTR|=0x80;//打开ADC电源
}