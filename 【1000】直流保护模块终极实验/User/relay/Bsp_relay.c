#include "Bsp_relay.h"

static int RELAY_CTL(int which, int status);

/* C99֧������ֵ�ṹ��
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//��ʼ��LED����ṹ��
struct relay_operations RELAY_fops = {RELAY_NUM , RELAY_CTL};

/**
 * @brief  RELAY_CTL RELAY����
 * @param  which-relay1 �ð��1���̵���  status 1-�� 0-�ر�  
 * @retval 0 ���óɹ���-1 ����ʧ��
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
 * @brief  RELAY_Init RELAY��ʼ��
 * @param  ��
 * @retval ��
 */
void RELAY_Init(void)
{
	P0M0|=0x08;				//P0M0 0.3=1
	P0M1&=0xF7;				//P0M1 0.3=0
	relay = 0;
}