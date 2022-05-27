#include "Bsp_uart.h"

static int LED_CTL(int which, int status);
static void U1SEND_C(unsigned char SEND_C);		//����1���͵��ַ����ݺ���
char putchar(char ch);			//�����ַ��ض�����

/* C99֧������ֵ�ṹ�� C89��֧��
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//��ʼ��LED����ṹ��
struct uart_operations UART_fops = {UART_NUM};

/**
 * @brief  UART1_Init() ����1��ʼ��
 * @param  ��
 * @retval ��
 */
void UART1_Init(void)
{
	SCON=0x50;			//181�ṹ���ɱ䲨���ʣ��������ݽ���
	AUXR|=0x01;			//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR|=0x04;			//��ʱ��ʱ��1Tģʽ
	T2L=BAUD_SET;		//���ö�ʱ��ʼֵ
	T2H=BAUD_SET>>8;//���ö�ʱ��ʼֵ
	AUXR|=0x10;			//��ʱ��2��ʼ��ʱ
	RI=0;TI=0;			//����������ݱ�־λ�ͷ������ݱ�־λ
}

/**
 * @brief  U1SEND_C ����1���͵��ַ�����	 ���β�SEND_C��Ϊ�����͵��ֽ�
 * @param  ��
 * @retval ��
 */
void U1SEND_C(unsigned char SEND_C)
{
	TI=0;				//���������ɱ�־λ
	SBUF=SEND_C;		//��������
	while(!TI);			//�ȴ����ݷ������
}

/**
 * @brief  char putchar(char ch) �����ַ��ض���
 * @param  ��
 * @retval ��
 */
char putchar(char ch)
{
	U1SEND_C((unsigned char)ch);//ͨ������1��������
	return ch;
}
