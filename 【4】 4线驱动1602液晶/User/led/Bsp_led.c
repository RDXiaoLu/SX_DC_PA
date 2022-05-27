#include "Bsp_led.h"

static int LED_CTL(int which, int status);

/* C99֧������ֵ�ṹ��
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//��ʼ��LED����ṹ��
struct led_operations LED_fops = {LED_NUM , LED_CTL};

/**
 * @brief  LED_CTL LED����
 * @param  which 1-4  D1 D2 D3 D4	 status 1-�� 0-��
 * @retval 0 ���óɹ���1 ����ʧ��
 */
static int LED_CTL(int which, int status)
{
	switch(which)
	{
		case 1:	LED1 = !status;break;
		case 2:	LED2 = !status;break;
		case 3:	LED3 = !status;break;
		case 4:	LED4 = !status;break;
		default: return -1;break;
	}
	return 0;
}

/**
 * @brief  LED_Init LED��ʼ��
 * @param  ��
 * @retval ��
 */
void LED_Init(void)
{
	P2M0|=0xF0;				//P2M0.0-3=0
	P2M1&=0xF0;				//P2M1.0-3=0
}