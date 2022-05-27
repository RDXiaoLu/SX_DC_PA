/*********************************************************************
 * ʵ�����ƣ�	ֱ������ģ�龿��ʵ��
 
 ��+	         ����+	           �̵���+
 
 ��ѹ�ɼ�+	     1602+	           �ڲ�EEPROM+
 
 �����߳�+	     ������ʱ�ָ�+     �����ʶ�ʱ�ָ�+			
 
 Ƿѹ���+		 ��ѹ���+			�������+
				  
 �����ʼ��
							
 * ѡ���鼮��	�ϲ�-������ǳ��STC8��ǿ��51��Ƭ�����׹��ԡ�2022�ѳ���
							�²�-������ǳ��STC8��ǿ��51��Ƭ��ʵս���ԡ�����д��
 * �鼮���ߣ�	��˳����� �廪��ѧ���������
 * �Ա����̣�	https://520mcu.taobao.com/
 * ʵ��ƽ̨��	˼�޵���STC8��ս����ϵ��С���ӿ�����
 * оƬ�ͺţ�	STC8H3K64S4-LQFP32������΢�������ֲ��STC����ϵ�е�Ƭ����
 * ʱ��˵����	Ƭ��IRC����24MHzʱ�ӣ�ʹ��STC-ISP���س���ʱ����
 * ʵ��˵����	��װ1602�ַ�Һ��ģ�飨ע��1��λ�ã���Ī�巴����������������������ϵ磬����ST
 C-ISP���ò����س���
 �߳�1���õ�ѹ������Χ��Ĭ��ֱ�Ӵ洢������IAP����ͨ�����ú궨�壬ѡ���Ƿ�����EEPOM�еĲ������򽫲����洢��EEPROM��
 �߳�2���õ���������Χ��Ĭ��ֱ�Ӵ洢������IAP����ͨ�����ú궨�壬ѡ���Ƿ�����EEPOM�еĲ������򽫲����洢��EEPROM��
 �߳�3���ù��ʱ�����Χ��Ĭ��ֱ�Ӵ洢������IAP����ͨ�����ú궨�壬ѡ���Ƿ�����EEPOM�еĲ������򽫲����洢��EEPROM��
 �߳�4����ʾ������+��ѹ+���ʣ������ϱȽϵ�������ѹ�������Ƿ�����趨��������������ϣ���򿪼̵����������Իָ���ʱ��������Ӧ״ָ̬ʾ�ơ�
 
 KEY1������תѡ��Ҫִ�еĹ��ܣ��̣߳�
 KEY2�����������̣߳���ʱҳ����ʾִ���̵߳��̺߳������ƣ�����ִ���̺߳������ٴε���KEY2������ѡ��ҳ�档
 
*********************************************************************/
#include "STC8H.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"  //�õ�pow 
#include "./led/Bsp_led.h"
#include "./key/Bsp_key.h"
#include "./lcd/Bsp_lcd.h"
#include "./adc/Bsp_adc.h"
#include "./uart/Bsp_uart.h"
#include "./relay/Bsp_relay.h"
#include "./lap/Bsp_lap.h"


/***************************�û��Զ�����������***********************/
#define OPEN 1
#define CLOSE 0

#define Max_Index 4 
#define Min_Index 1
//#define NULL (void *)0

int  List_Index = 2; 	//Һ�����˵�����
int  List_Flag = 0;	//������־ 1����Խ���ѡ�� 0��ֻͣ���ڲ˵�ѡ��

/* �����߳̽ṹ�� */
typedef struct Thread{
	int PID;   					//���̺�
	char name[17];				//��ʾ����
	void (*Thread_Fun)(void);	//�̺߳���
	struct Thread *Next;
}Thread,*PThread;

PThread g_pThread = NULL; //����ͷ�ڵ�
unsigned char g_flag = 0;  //�߳��ж�ʹ�ܱ�־
#define OPEN 1
#define CLOSE 0

int V_MAX =  35;	  /* �����޶���Χ���ֵ */
int V_MIN =  25;	  /* �����޶���Χ��Сֵ */
static unsigned int AD_Vtemp[10]={0};//װ��10��ADC��������
static unsigned int AD_val=0;				
char v_maxstr[4];	  //��ʼ��
char v_minstr[4];
char vol_str[15];


int I_MAX =  10;	  /* �����޶���Χ���ֵ */
int I_MIN =  0;	  /* �����޶���Χ��Сֵ */			
char i_maxstr[4];	  //��ʼ��
char i_minstr[4];
char cur_str[15];

int P_MAX =  30;	  /* �����޶���Χ���ֵ */
int P_MIN =  0;	  /* �����޶���Χ��Сֵ */			
char p_maxstr[4];	  //��ʼ��
char p_minstr[4];
char pow_str[10];

double GETcurrent = 0.0;
double GETvoltage = 0.0;
double Power = 0.0;

#define Measure_I 0x00
#define Measure_V 0x02
/*****************************������������***************************/
void delay(unsigned int Count);			//��ʱ����


void OS_Init(void);   			          //Һ����ʾ�˵���ܳ�ʼ��
void Register_Threads(void);              //�̵߳ǼǺ���
void Thread_List(PThread ptThread);       //�߳�������
PThread Thread_Index(int Index);          //�߳���������

void Thread_one(void);				//�߳�1����
void Thread_two(void);				//�߳�2����
void Thread_three(void);			//�߳�3����
void Thread_four(void);				//�߳�4����
void Thread_five(void);				//�߳�5����

extern void delay(unsigned int Count); /* ������ʱ */
static void OS_Init(void);	 /* ϵͳӲ����ʼ�� */
void Timer2Init(void); /* �Իָ���ʱ�� */

/**
 * @brief  AVG_AD_Vtemp() �����ѹ��λ��ƽ��ֵ�˲�����
 * @param  ��
 * @retval ��
 */
void AVG_AD_Vtemp(void)
{
	unsigned char i,j;								//���������������ѭ������i��j
	unsigned int temp;							//�����м䡰�ݴ桱����temp
	for(i=10;i>=1;i--)		//���ѭ��
	{
		for(j=0;j<(i-1);j++)//�ڲ�ѭ��
		{
			if(AD_Vtemp[j]>AD_Vtemp[j+1])//��ֵ�Ƚ�
			{
				temp=AD_Vtemp[j];//��ֵ��λ
				AD_Vtemp[j]=AD_Vtemp[j+1];
				AD_Vtemp[j+1]=temp;
			}
		}
	}
	for(i=2;i<=7;i++)				//ȥ��2�����ȥ��2�����
	AD_val+=AD_Vtemp[i];		//���м�6����ֵ�ۼ�
	AD_val=(unsigned int)(AD_val/6);	//�ۼӺ���ƽ��ֵ
}

/******************************����������****************************/
void main(void)
{
	PThread ptThread = NULL;  /* ��ʼ������ */

	OS_Init(); /* ϵͳӲ����ʼ�� */

	printf("--------------------------------------\r\n");
	printf("��ӭ����˼�޵���ֱ��������������ռ�ʵ��");	
	printf("--------------------------------------\r\n");

	while(1)
	{
		/* �����߳�����ִ����Ӧ���� */
		printf("List_Index = %d\n",List_Index);	
		ptThread = 	Thread_Index(List_Index);	

		
		if(List_Flag)	    //EC11���� ִ�е�ǰѡ���̵߳��̺߳���
		{
			ptThread->Thread_Fun();
			List_Index = 1;  //�˳��̺߳����� ���߳�1��ʼѡ��
		}
		
		LCD_fops.dis_line(ptThread->name,1); //ˢ����ʾ
		LCD_fops.dis_line("                ",2);
	}	
}

/**
 * @brief  OS_Init Ӳ����ʼ��
 * @param  ��
 * @retval ��
 */
void OS_Init(void)
{
	LED_Init();	
	KEY_Init();
	RELAY_Init();
	LCD_Init();
	ADC_Init();
	UART1_Init();
	Timer2Init();		//2����@24.000MHz
	//ʹ��P3^2 P3^3���ⲿ�ж�
	
	IT1 = 1; //ʹ�� INT1 �½����ж�
	EX1 = 1; //ʹ�� INT1 �ж�
	EA = 1;   //�����ж�

	LCD_fops.dis_line(" Mode selection ",0);
	LCD_fops.dis_line("--    MODE    --",1);
	List_Index = 1;
	delay(5);
	Register_Threads();
	
	IT0 = 1;  //ʹ��INT0�½����ж�
	EX0 = 1;  //ʹ��INT0�ж�

}
/*
	�߳�����
*/
PThread Thread_Index(int Index)
{
	PThread ptTmp = g_pThread;
	while(ptTmp)
	{
		if(ptTmp->PID == Index) 
			return ptTmp;
		ptTmp = ptTmp->Next;
	}
	return NULL;
}
/*
�̳߳�ʼ�� 
struct Thread tThread1 = {
//		.PID = 1,
//		.name = "--  thread_1x  --",
//		.Thread = Thread_one,
//};
//�����ø÷�������ֵ�ṹ�壬C99��׼

/*
	�̵߳Ǽ�  	  ע�⣡���߳�5�����ã������������ͷ
*/
void Register_Threads(void)
{
	Thread tThread5 = {5, "--            --", Thread_five};
	Thread tThread4 = {4, "--  thread_4  --", Thread_four};
	Thread tThread2 = {2, "--  thread_2  --", Thread_two};
	Thread tThread3 = {3, "--  thread_3  --", Thread_three};
	Thread tThread1 = {1, "--  thread_1  --", Thread_one};	
	
	Thread_List(&tThread5);
	Thread_List(&tThread4);
	Thread_List(&tThread3);
	Thread_List(&tThread2);
	Thread_List(&tThread1);
	

}

/*
	�߳������� ͷ�巨
*/
void Thread_List(PThread ptThread)
{
	ptThread->Next = g_pThread;
	g_pThread = ptThread;
}
/*
	
*/
/**
 * @brief  Thread_one �̺߳�������ģ��
 * @param  ��
 * @retval ��
 */
void Thread_one(void)
{
	 unsigned char index = 0;
	 LCD_fops.dis_line("-protect V set -",1);
	 LCD_fops.write(0x0d,0);   //�������ַ���˸
	 sprintf(v_maxstr, "%2d",V_MAX);  		  /* ����ת�����ַ�������ڶ�Ӧ�ַ������� */
	sprintf(v_minstr, "%2d",V_MIN); 
	 /* Һ����ʾˢ�� */
	LCD_fops.dis_str(0,2,v_minstr);
	LCD_fops.dis_char(3,2,'<');
	LCD_fops.dis_str(4,2,"V__range");	//��x=4 y=2 ��λ�ÿ�ʼ��ʾ�ַ���vol_str	
	LCD_fops.dis_char(12,2,'<');
	LCD_fops.dis_str(14,2,v_maxstr);
	 while(1)
	 {
		if(KEY_fops.read(7) == 0)		//���ѡ����������
		{
			delay(20);
			if(KEY_fops.read(7) == 0)
			{
				if(index >= 0 && index <=3) index ++;
				if(index < 0) index = 3;
				if(index > 3) index = 0;
				while(!(KEY_fops.read(7)));
			}
		}
		if(KEY_fops.read(3) == 0)
		{
			delay(20);
			if(KEY_fops.read(3) == 0)	 //��Ӧ�����ж�
			{
				switch(index)
				{
					 case 0: V_MIN+=10;break; //V_MAX	V_MIN
					 case 1: V_MIN+=1;break;
					 case 2: V_MAX+=10;break;
					 case 3: V_MAX+=1;break;
				}
				while(!(KEY_fops.read(3)));
			}
		}
		if(KEY_fops.read(4) == 0)
		{
			delay(20);
			if(KEY_fops.read(4) == 0)	 //��Ӧ�����ж�
			{
				switch(index)
				{
					 case 0: V_MIN-=10;break; //V_MAX	V_MIN
					 case 1: V_MIN-=1;break;
					 case 2: V_MAX-=10;break;
					 case 3: V_MAX-=1;break;
				}
				while(!(KEY_fops.read(4)));
			}
		}
		if(V_MIN < 0) V_MIN = 0;
		if(V_MIN > 100) V_MIN = 99;
		if(V_MAX > 100) V_MAX = 99;
		if(V_MAX < 0)  V_MAX = 0;
		/* �����ò����洢���ڲ�EEPROM�� */
#if		1
		IAP_fops.erase(Region_1);
		IAP_fops.program(Region_1,V_MIN);
		IAP_fops.erase(Region_2);
		IAP_fops.program(Region_2,V_MAX);
#endif
		sprintf(v_maxstr, "%2d",V_MAX);  		  /* ����ת�����ַ�������ڶ�Ӧ�ַ������� */
		sprintf(v_minstr, "%2d",V_MIN); 
			
		switch(index)
		{
			 case 0: LCD_fops.dis_char(0,2,v_minstr[0]);break; //V_MAX	V_MIN
			 case 1: LCD_fops.dis_char(1,2,v_minstr[1]);break;
			 case 2: LCD_fops.dis_char(14,2,v_maxstr[0]);break;
			 case 3: LCD_fops.dis_char(15,2,v_maxstr[1]);break;
			 default: LCD_fops.dis_char(0,2,v_minstr[0]);break;
		}
		
	 	if(!List_Flag) {LCD_fops.write(0x0c,0); return;} //�˳����߳�	
	 }
}

void Thread_two(void)
{
	unsigned char index = 0;
	 LCD_fops.dis_line("-protect I set -",1);
	 LCD_fops.write(0x0d,0);   //�������ַ���˸
	 sprintf(i_maxstr, "%2d",I_MAX);  		  /* ����ת�����ַ�������ڶ�Ӧ�ַ������� */
	sprintf(i_minstr, "%2d",I_MIN); 
	 /* Һ����ʾˢ�� */
	LCD_fops.dis_str(0,2,i_minstr);
	LCD_fops.dis_char(3,2,'<');
	LCD_fops.dis_str(4,2,"I__range");	//��x=4 y=2 ��λ�ÿ�ʼ��ʾ�ַ���vol_str	
	LCD_fops.dis_char(12,2,'<');
	LCD_fops.dis_str(14,2,i_maxstr);
	 while(1)
	 {
		if(KEY_fops.read(7) == 0)		//���ѡ����������
		{
			delay(20);
			if(KEY_fops.read(7) == 0)
			{
				if(index >= 0 && index <=3) index ++;
				if(index < 0) index = 3;
				if(index > 3) index = 0;
				while(!(KEY_fops.read(7)));
			}
		}
		if(KEY_fops.read(3) == 0)
		{
			delay(20);
			if(KEY_fops.read(3) == 0)	 //��Ӧ�����ж�
			{
				switch(index)
				{
					 case 0: I_MIN+=10;break; //V_MAX	V_MIN
					 case 1: I_MIN+=1;break;
					 case 2: I_MAX+=10;break;
					 case 3: I_MAX+=1;break;
				}
				while(!(KEY_fops.read(3)));
			}
		}
		if(KEY_fops.read(4) == 0)
		{
			delay(20);
			if(KEY_fops.read(4) == 0)	 //��Ӧ�����ж�
			{
				switch(index)
				{
					 case 0: I_MIN-=10;break; //V_MAX	V_MIN
					 case 1: I_MIN-=1;break;
					 case 2: I_MAX-=10;break;
					 case 3: I_MAX-=1;break;
				}
				while(!(KEY_fops.read(4)));
			}
		}
		if(I_MIN < 0) I_MIN = 0;
		if(I_MIN > 10) I_MIN = 10;
		if(I_MAX > 10) I_MAX = 10;
		if(I_MAX < 0)  I_MAX = 0;
		/* �����ò����洢���ڲ�EEPROM�� */
#if		1
		IAP_fops.erase(Region_3);
		IAP_fops.program(Region_3,I_MIN);
		IAP_fops.erase(Region_4);
		IAP_fops.program(Region_4,I_MAX);
#endif
		sprintf(i_maxstr, "%2d",I_MAX);  		  /* ����ת�����ַ�������ڶ�Ӧ�ַ������� */
		sprintf(i_minstr, "%2d",I_MIN); 
			
		switch(index)
		{
			 case 0: LCD_fops.dis_char(0,2,i_minstr[0]);break; //V_MAX	V_MIN
			 case 1: LCD_fops.dis_char(1,2,i_minstr[1]);break;
			 case 2: LCD_fops.dis_char(14,2,i_maxstr[0]);break;
			 case 3: LCD_fops.dis_char(15,2,i_maxstr[1]);break;
			 default: LCD_fops.dis_char(0,2,i_minstr[0]);break;
		}
		
	 	if(!List_Flag) {LCD_fops.write(0x0c,0); return;} //�˳����߳�	
	 }
}

void Thread_three(void)
{
	unsigned char index = 0;
	 LCD_fops.dis_line("-protect P set -",1);
	 LCD_fops.write(0x0d,0);   //�������ַ���˸
	 sprintf(p_maxstr, "%2d",P_MAX);  		  /* ����ת�����ַ�������ڶ�Ӧ�ַ������� */
	sprintf(p_minstr, "%2d",P_MIN); 
	 /* Һ����ʾˢ�� */
	LCD_fops.dis_str(0,2,p_minstr);
	LCD_fops.dis_char(3,2,'<');
	LCD_fops.dis_str(4,2,"P__range");	//��x=4 y=2 ��λ�ÿ�ʼ��ʾ�ַ���vol_str	
	LCD_fops.dis_char(12,2,'<');
	LCD_fops.dis_str(14,2,p_maxstr);
	 while(1)
	 {
		if(KEY_fops.read(7) == 0)		//���ѡ����������
		{
			delay(20);
			if(KEY_fops.read(7) == 0)
			{
				if(index >= 0 && index <=3) index ++;
				if(index < 0) index = 3;
				if(index > 3) index = 0;
				while(!(KEY_fops.read(7)));
			}
		}
		if(KEY_fops.read(3) == 0)
		{
			delay(20);
			if(KEY_fops.read(3) == 0)	 //��Ӧ�����ж�
			{
				switch(index)
				{
					 case 0: P_MIN+=10;break; //V_MAX	V_MIN
					 case 1: P_MIN+=1;break;
					 case 2: P_MAX+=10;break;
					 case 3: P_MAX+=1;break;
				}
				while(!(KEY_fops.read(3)));
			}
		}
		if(KEY_fops.read(4) == 0)
		{
			delay(20);
			if(KEY_fops.read(4) == 0)	 //��Ӧ�����ж�
			{
				switch(index)
				{
					 case 0: P_MIN-=10;break; //V_MAX	V_MIN
					 case 1: P_MIN-=1;break;
					 case 2: P_MAX-=10;break;
					 case 3: P_MAX-=1;break;
				}
				while(!(KEY_fops.read(4)));
			}
		}
		if(P_MIN < 0) P_MIN = 0;
		if(P_MIN > 10) P_MIN = 10;
		if(P_MAX > 10) P_MAX = 10;
		if(P_MAX < 0)  P_MAX = 0;
		/* �����ò����洢���ڲ�EEPROM�� */
#if		1
		IAP_fops.erase(Region_5);
		IAP_fops.program(Region_5,I_MIN);
		IAP_fops.erase(Region_6);
		IAP_fops.program(Region_6,I_MAX);
#endif
		sprintf(i_maxstr, "%2d",P_MAX);  		  /* ����ת�����ַ�������ڶ�Ӧ�ַ������� */
		sprintf(i_minstr, "%2d",P_MIN); 
			
		switch(index)
		{
			 case 0: LCD_fops.dis_char(0,2,p_minstr[0]);break; //V_MAX	V_MIN
			 case 1: LCD_fops.dis_char(1,2,p_minstr[1]);break;
			 case 2: LCD_fops.dis_char(14,2,p_maxstr[0]);break;
			 case 3: LCD_fops.dis_char(15,2,p_maxstr[1]);break;
			 default: LCD_fops.dis_char(0,2,p_minstr[0]);break;
		}
		
	 	if(!List_Flag) {LCD_fops.write(0x0c,0); return;} //�˳����߳�	
	 }
}

void Thread_four(void) 
{
	bit flag;
	unsigned char i;
	LCD_fops.dis_line("-Display param -",1);

	/* �Ƿ�ʹ��IAP�д洢�Ĳ��� ��һ��ʹ����Ҫ�������߳����ò��� */
#if	1
	V_MIN = (int)IAP_fops.read(Region_1);
	V_MAX = (int)IAP_fops.read(Region_2);
	I_MIN = (int)IAP_fops.read(Region_3);
	I_MAX = (int)IAP_fops.read(Region_4);
	P_MIN = (int)IAP_fops.read(Region_5);
	P_MAX = (int)IAP_fops.read(Region_6);
#endif	

	while(1)
	{
		RELAY_fops.relay_ctl(1,1);	
		if(flag)
		{
			for(i=0; i<10; i++)
			{
				AD_Vtemp[i] = ADC_fops.adc_get(Measure_I);		
			}
	
			AVG_AD_Vtemp();

			GETcurrent=(double)(AD_val*0.060791015625/111*5);	//����2.5V/4096����õ��Ľ��
			//������� ����matlab�����������
			GETcurrent = -0.0062*pow(GETcurrent,6.0) + 0.0776*pow(GETcurrent,5.0) - 0.3777*pow(GETcurrent,4.0)+0.8987*pow(GETcurrent,3.0) -1.0724*pow(GETcurrent,2.0) + 1.4202* GETcurrent + 0.0714;
			AD_val = 0;

			//sprintf(cur_str, "%.1f",GETcurrent-(double)(((int)(GETcurrent*100))%10)/100);   //����1λС���������������λ
			sprintf(cur_str, "%.2f",GETcurrent);
			GETcurrent = atof(cur_str);	//�ַ���ת����double����
			if(I_MAX >= GETcurrent && I_MIN <= GETcurrent)
			{
				RELAY_fops.relay_ctl(1,OPEN);	 /* �̵������� */
				LED_fops.led_ctl(2,CLOSE);
				AUXR &= 0xEF;  	//��ʹ�ܶ�ʱ��2
				AUXR |= 0x10;  	//ʹ�ܶ�ʱ��2
				IE2 &= 0xFB;  //�رն�ʱ��2�ж�
			}
			else if(I_MAX < GETcurrent || I_MIN > GETcurrent)  //�������������
			{
				LED_fops.led_ctl(2,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* �̵������� */
				AUXR |= 0x10;  	//ʹ�ܶ�ʱ��2
				IE2 = 0x04;  //������ʱ��2�ж�
			}
			LCD_fops.dis_str(0,1,"I:");
			LCD_fops.dis_str(2,1,cur_str);	//��x=4 y=2 ��λ�ÿ�ʼ��ʾ�ַ���vol_str	   
			LCD_fops.dis_str(6,1,"A "); 
			//printf("out Cur: %.1f\r\n",GETcurrent);	
		}
		else{
			for(i=0; i<10; i++)
			{
				AD_Vtemp[i] = ADC_fops.adc_get(Measure_V);		
			}
			AVG_AD_Vtemp();
	
			GETvoltage=(double)(AD_val*0.0006103515625*40);	//����5V/4096����õ��Ľ��
			AD_val = 0;
			if(V_MAX >= GETvoltage && V_MIN <= GETvoltage)
			{
				RELAY_fops.relay_ctl(1,OPEN);	 /* �̵������� */
				LED_fops.led_ctl(3,CLOSE);       
				LED_fops.led_ctl(4,CLOSE);
			}
			else if(V_MAX < GETvoltage )  //��ѹ
			{
				LED_fops.led_ctl(3,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* �̵������� */
			}
			else if(V_MIN > GETvoltage )  //Ƿѹ
			{
				LED_fops.led_ctl(4,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* �̵������� */
			}
			//memset(AD_Vtemp, 0, sizeof(AD_Vtemp));
			sprintf(vol_str, "%.2f",GETvoltage);   //�õ���ѹ ���� �Ŵ���40
			GETvoltage = atof(vol_str);	//�ַ���ת����double����
			LCD_fops.dis_str(8,1,"V:");
			LCD_fops.dis_str(10,1,vol_str);	//��x=8 y=8 ��λ�ÿ�ʼ��ʾ�ַ���vol_str	   
			LCD_fops.dis_char(15,1,'V'); 	
			//printf("out Vol: %.2f\r\n",GETvoltage);
		}
		Power = GETvoltage * GETcurrent;
		if(P_MAX >= Power && P_MIN <= Power)
			{
				RELAY_fops.relay_ctl(1,OPEN);	 /* �̵������� */
				LED_fops.led_ctl(1,CLOSE);
				AUXR &= 0xEF;  	//��ʹ�ܶ�ʱ��2
				AUXR |= 0x10;  	//ʹ�ܶ�ʱ��2
				IE2 &= 0xFB;  //�رն�ʱ��2�ж�
			}
			else if(P_MAX < GETcurrent || P_MIN > GETcurrent)  //�����㹦������
			{
				LED_fops.led_ctl(1,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* �̵������� */
				AUXR |= 0x10;  	//ʹ�ܶ�ʱ��2
				IE2 = 0x04;  //������ʱ��2�ж�
			}
		sprintf(pow_str, "%.2f",Power);   //�õ�����
		LCD_fops.dis_str(0,2,"P:");
		LCD_fops.dis_str(2,2,pow_str);
		LCD_fops.dis_char(7,2,'W');
		flag = !flag;
		if(!List_Flag) return; //�˳����߳�		
	}
}

void Thread_five(void) 
{
	while(1)
	{
		LCD_fops.dis_line("-- run four  --",1);
		if(!List_Flag) return; //�˳����߳�		
	}
}


/********************************************************************/
//��ʱ����delay(u16 Count)�����β�Count,�޷���ֵ
/********************************************************************/
void delay(unsigned int Count)
{
  unsigned char i,j;
  while (Count--)//Count�βο�����ʱ����
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}


/* KEY1 ��ť�жϺ��� */
void INT0_Isr(void) interrupt 0
{

	if(KEY_fops.read(1)==0)						//����ȷ���Ƿ�
	{	 
		 if(List_Index<Max_Index)List_Index+=1;
		 else  List_Index = Min_Index;
		 while(!KEY_fops.read(1));
	}					

}

/**
 * @brief  Timer2Init ��ʱ��2��ʼ��
 * @param  ��
 * @retval ��
 */
void Timer2Init(void)		//2����@24.000MHz
{
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0x80;		//���ö�ʱ��ֵ
	T2H = 0x44;		//���ö�ʱ��ֵ	
}




/* EC11 �����жϺ��� */
void INT1_Isr() interrupt 2
{
	
	if(KEY_fops.read(2)==0)						//����ȷ���Ƿ�
	{
		 List_Flag = !List_Flag;				//�ж�Ϊ������Ϊ
	}
	while(!KEY_fops.read(2));
}

/**
 * @brief  TM2_Isr ��ʱ��2 �Իָ��ж�
 * @param  ��
 * @retval ��
 */
void TM2_Isr() interrupt 12 
{
	static unsigned int i = 0;
	if(i>=4000)	  //��Լ8���Իָ�һ��
	{
		RELAY_fops.relay_ctl(1,OPEN); //�Իָ�
		i = 0;
	    AUXR &= 0xEF;  	//��ʹ�ܶ�ʱ��2
	}
	i++;
}