#include "Bsp_adc.h"

extern void delay(unsigned int Count); /* ������ʱ */

static int ADC_GET(unsigned char which);

/* C99֧������ֵ�ṹ��
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//��ʼ��LED����ṹ��
struct adc_operations ADC_fops = {ADC_NUM , ADC_GET};

/**
 * @brief  LED_CTL LED����
 * @param  which 1-4  D1 D2 D3 D4	 status 1-�� 0-��
 * @retval 0 ���óɹ���1 ����ʧ��
 */
static int ADC_GET(unsigned char which)
{
	int AD_Vtemp = 0;
	ADC_CONTR &= 0xf0;
	ADC_CONTR |= which;
	ADC_CONTR|=0x40;					//����ADCת��
	delay(10);								//��ʱ�ȴ�ת�����
	while(!(ADC_CONTR&0x20));	//�ȴ�ADCת������
	ADC_CONTR &= 0xDF;					//����ADCת��������־λ
	AD_Vtemp = (unsigned int)ADC_RES<<4;
	AD_Vtemp |= (unsigned int)ADC_RESL>>4;//����������ʽ�µ�ADC���
	return AD_Vtemp;
}

/**
 * @brief  ADC_Init ADC��ʼ��
 * @param  ��
 * @retval ��
 */
void ADC_Init(void)
{
	ADC_CONTR=0x02;	//ѡ��P1.2������ΪADCģ���ѹ��������
	//����P1.2Ϊ��������ģʽ
	P1M0&=0xFB;			//P0M0.1=0
	P1M1|=0x04;			//P0M1.1=1
	ADCCFG=0x0F;		//ת������������ADCת��ʱ��ΪSYSCLK/2/16
	ADC_CONTR|=0x80;//��ADC��Դ
}