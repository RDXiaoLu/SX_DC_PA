
#include "Bsp_key.h"
//#include "./../led/Bsp_led.h"		/* ./��ʾ��ǰĿ¼  ../��ʾ��һ��Ŀ¼ */
/*****************************������������***************************/
static int KEY_READ(int which);

/* C99֧������ֵ�ṹ��
struct led_operations LED{
	.num = KEY_NUM;
	.read = KEY_READ;	
};
*/

//��ʼ��LED����ṹ��
struct key_operations KEY_fops = {KEY_NUM , KEY_READ};

/**
 * @brief  KEY_READ KEY״̬��ȡ
 * @param  which 1-8  ѡȡ��Ӧ����
 * @retval ���ŵ�ƽ״̬
 */
static int KEY_READ(int which)
{
	int temp = 0;
	switch(which)
	{
		case 1: return KEY1;
		case 2:	return KEY2;
		case 3:	return KEY3;
		case 4:	return KEY4;
		case 5: return KEY5;
		case 6:	return KEY6;
		case 7:	return KEY7;
		case 8:	return KEY8;
		default: return -1;break;
	}
	return 0;
}

/**
 * @brief  KEY_Init KEY��ʼ��
 * @param  ��
 * @retval ��
 */
void KEY_Init(void)
{
	P3M0&=0x03;				//P3M0.2-7=0
	P3M1&=0x03;				//P3M1.2-7=0
	P5M0&=0xDF;
	P5M1&=0xDF;
	P1M0&=0x7F;
	P1M1&=0x7F;
}