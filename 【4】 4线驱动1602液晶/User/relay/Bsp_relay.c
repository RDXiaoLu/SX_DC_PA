#include "Bsp_relay.h"

static int RELAY_CTL(int which, int status);

/* C99支持乱序赋值结构体
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//初始化LED外设结构体
struct relay_operations RELAY_fops = {RELAY_NUM , RELAY_CTL};

/**
 * @brief  RELAY_CTL RELAY控制
 * @param  which-relay1 该板仅1个继电器  status 1-打开 0-关闭  
 * @retval 0 设置成功-1 设置失败
 */
static int RELAY_CTL(int which, int status)
{
	switch(which)
	{
		case 1:	relay = status;break;
		default: return -1;break;
	}
	return 0;
}

/**
 * @brief  RELAY_Init RELAY初始化
 * @param  无
 * @retval 无
 */
void RELAY_Init(void)
{
	P0M0|=0x08;				//P0M0 0.3=1
	P0M1&=0xF7;				//P0M1 0.3=0
	relay = 0;
}