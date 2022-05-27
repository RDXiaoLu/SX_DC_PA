#include "Bsp_lap.h"
#include "intrins.h"


static char IapRead(int addr); 
static void IapProgram(int addr, char dat);
static void IapErase(int addr);
static void Iap_Int_Program(int addr, int dat); 
static int Iap_Int_Read(int addr);
static void Iap_Int_Erase(int addr); 
/* C99֧������ֵ�ṹ��
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//��ʼ��LED����ṹ��
struct iap_operations IAP_fops = {
	IAP_NUM,
	IapProgram,
	IapRead,
	IapErase,
	Iap_Int_Program,
	Iap_Int_Read,	
	Iap_Int_Erase,						 
								 };

/**
 * @brief  IapIdle Iap����
 * @param  which 1-4  D1 D2 D3 D4	 status 1-�� 0-��
 * @retval ��
 */
static void IapIdle(void) 
{ 
	IAP_CONTR = 0; //�ر� IAP ����
	IAP_CMD = 0; //�������Ĵ���
	IAP_TRIG = 0; //��������Ĵ���
	IAP_ADDRH = 0x80; //����ַ���õ��� IAP ����
	IAP_ADDRL = 0; 
}

/**
 * @brief  IapRead Iap��ȡ
 * @param  addr-��ȡ��ַ
 * @retval ��
 */
static char IapRead(int addr) 
{ 
 	 char dat; 
 
	 IAP_CONTR = 0x80; //ʹ�� IAP
	 IAP_TPS = 24; //���ò����ȴ����� 24MHz
	 IAP_CMD = 1; //���� IAP ������
	 IAP_ADDRL = addr; //���� IAP �͵�ַ
	 IAP_ADDRH = addr >> 8; //���� IAP �ߵ�ַ
	 IAP_TRIG = 0x5a; //д��������(0x5a)
	 IAP_TRIG = 0xa5; //д��������(0xa5)
	 _nop_(); 
	 dat = IAP_DATA; //�� IAP ����
	 IapIdle(); //�ر� IAP ����
 	 return dat; 
}

/**
 * @brief  IapErase Iapд��
 * @param  addr-д���ַ  dat-д������
 * @retval ��
 */
static void IapProgram(int addr, char dat) 
{ 
	 IAP_CONTR = 0x80; //ʹ�� IAP
	 IAP_TPS = 24; //���ò����ȴ����� 24MHz
	 IAP_CMD = 2; //���� IAP д����
	 IAP_ADDRL = addr; //���� IAP �͵�ַ
	 IAP_ADDRH = addr >> 8; //���� IAP �ߵ�ַ
	 IAP_DATA = dat; //д IAP ����
	 IAP_TRIG = 0x5a; //д��������(0x5a)
	 IAP_TRIG = 0xa5; //д��������(0xa5)
	 _nop_(); 
	 IapIdle(); //�ر� IAP ����
}			
/**
 * @brief  IapErase Iap����
 * @param  ��
 * @retval ��
 */

static void IapErase(int addr) 
{ 
	 IAP_CONTR = 0x80; //ʹ�� IAP
	 IAP_TPS = 24; //���ò����ȴ����� 24MHz
	 IAP_CMD = 3; //���� IAP ��������
	 IAP_ADDRL = addr; //���� IAP �͵�ַ
	 IAP_ADDRH = addr >> 8; //���� IAP �ߵ�ַ
	 IAP_TRIG = 0x5a; //д��������(0x5a)
	 IAP_TRIG = 0xa5; //д��������(0xa5)
	 _nop_(); //
	 IapIdle(); //�ر� IAP ����
}

/**
 * @brief  Iap_Int_Program Iapд������
 * @param  dat-д��int��������   addr
 * @retval ��
 */
static void Iap_Int_Program(int addr, int dat) 
{
	IapProgram(addr,  (char)(dat));
	IapProgram(addr+1,(char)(dat>>8));
	IapProgram(addr+2,(char)(dat>>16));
	IapProgram(addr+3,(char)(dat>>24)); 
}

static int Iap_Int_Read(int addr) 
{
	int temp = 0;
	temp = temp | (int)(IapRead(addr));
	temp = temp | ((int)(IapRead(addr+1))<<8);
	temp = temp | ((int)(IapRead(addr+2))<<16);
	temp = temp | ((int)(IapRead(addr+3))<<24);
	return temp;
}

/**
 * @brief  Iap_Int_Erase Iap��������
 * @param  addr-������ַ
 * @retval ��
 */
static void Iap_Int_Erase(int addr) 
{
	IapErase(addr);
	IapErase(addr+1); 
	IapErase(addr+2);
	IapErase(addr+3);
}
