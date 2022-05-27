/*********************************************************************
 * ʵ�����ƣ�	EC11+1602Һ�����׶༶�˵�ʵ��
 * ѡ���鼮��	�ϲ�-������ǳ��STC8��ǿ��51��Ƭ�����׹��ԡ�2022�ѳ���
							�²�-������ǳ��STC8��ǿ��51��Ƭ��ʵս���ԡ�����д��
 * �鼮���ߣ�	��˳����� �廪��ѧ���������
 * �Ա����̣�	https://520mcu.taobao.com/
 * ʵ��ƽ̨��	˼�޵���STC8��ս����ϵ��С���ӿ�����
 * оƬ�ͺţ�	STC8H8K64U-LQFP64������΢�������ֲ��STC����ϵ�е�Ƭ����
 * ʱ��˵����	Ƭ��IRC����24MHzʱ�ӣ�ʹ��STC-ISP���س���ʱ����
 * ʵ��˵����	�ȶϵ�����������07-EB��06-EA��05-EC��·ñ����װ1602�ַ�
 Һ��ģ�飨ע��1��λ�ã���Ī�巴����������������������ϵ磬����ST
 C-ISP���ò����س���
 Һ����һ����ʾ��
 Һ���ڶ�����ʾ��
 EC11������תѡ��Ҫִ�еĹ��ܣ��̣߳�
 EC11�����������̣߳���ʱҳ����ʾִ���̵߳��̺߳������ƣ�����ִ���̺߳������ٴε���EC11������ѡ��ҳ�档
 ע�⣺ EC11_B �ӵ� P3^2���ţ�����   EC11_C �ӵ� P3^3���� 
*********************************************************************/
#include	"STC8H.h"	//����оƬ��ͷ�ļ�
#include    "stdio.h"
#include    "stdlib.h"
/***************************�����������Ͷ���*************************/
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
typedef unsigned char  uint8_t;
typedef unsigned int   uint16_t;
typedef unsigned long  uint32_t;
/***************************�˿�/���Ŷ�������************************/
sbit EC11_B=P0^7; 	//EC11��B���ţ����壩
sbit EC11_A=P3^2; 	//EC11��A���ţ����壩		  EC11_B �ӵ� P3^2���� ����INT0�ⲿ�½����ж�
sbit EC11_C=P3^3; 	//EC11��E���ţ��ڲ�������	 ����INT1�ⲿ�½����ж�
sbit LCDRS=P7^4;		//LCD1602����/����ѡ��˿�
sbit LCDEN=P7^6;		//LCD1602ʹ���źŶ˿�
sbit LCDRW=P7^5;    //LCD1602��д���ƶ˿�
#define LCDDATA  P2	//LCD1602���ݶ˿�DB0��DB7
/***************************�û��Զ�����������***********************/
#define Max_Index 3 
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

/*****************************������������***************************/
void delay(u16 Count);			//��ʱ����
void LCD1602_Write(u8 cmdordata,u8 writetype);
//д��Һ��ģ����������ݺ���
void LCD1602_Init(void);       //Һ����ʼ������
void LCD1602_Dis_Line(char *str, int y);		//1602����ʾ����

void OS_Init(void);   			          //Һ����ʾ�˵���ܳ�ʼ��
void Register_Threads(void);              //�̵߳ǼǺ���
void Thread_List(PThread ptThread);       //�߳�������
PThread Thread_Index(int Index);          //�߳���������

void Thread_one(void);				//�߳�1����
void Thread_two(void);				//�߳�2����
void Thread_three(void);			//�߳�3����
void Thread_four(void);				//�߳�4����
/***************************���Խӿ�***********************/
#define SYSCLK 24000000UL		//ϵͳʱ��Ƶ��ֵ
#define BAUD_SET  (65536-SYSCLK/9600/4)//�������趨����� 
/*****************************������������***************************/								   
void UART1_Init(void);			//����1��ʼ������
void U1SEND_C(u8 SEND_C);		//����1���͵��ַ����ݺ���
char putchar(char ch);			//�����ַ��ض�����
/******************************����������****************************/
void main(void)
{
	PThread ptThread = NULL;

	OS_Init();


	printf("--------------------------------------\r\n");
	printf("��UART1��:MOD1 24MHz 9600bps\r\n");	
	printf("��UART1��:RXD/P3.0, TXD/P3.1\r\n");	
	printf("--------------------------------------\r\n");

	while(1)
	{
		
		printf("List_Index = %d\n",List_Index);	
		ptThread = 	Thread_Index(List_Index);	

		
		if(List_Flag)	    //EC11���� ִ�е�ǰѡ���̵߳��̺߳���
		{
			ptThread->Thread_Fun();
			List_Index = 1;  //�˳��̺߳����� ���߳�1��ʼѡ��
		}
		
		LCD1602_Dis_Line(ptThread->name,1); //ˢ����ʾ
	}	
}

void OS_Init(void)
{
	//����P7.4-6Ϊ׼˫��/������ģʽ
	P7M0&=0x8F;				//P7M0.4-6=0
	P7M1&=0x8F;				//P7M1.4-6=0
	//����P2Ϊ׼˫��/������ģʽ
	P2M0=0x00;				//P2M0.0-7=0
	P2M1=0x00;				//P2M1.0-7=0
	//����P0.5-7Ϊ׼˫��/������ģʽ
	P0M0&=0x1F;				//P0M0.5-7=0
	P0M1&=0x1F;				//P0M1.5-7=0
	delay(5);					//�ȴ�I/Oģʽ�����ȶ�
	//ʹ��P3^2 P3^3���ⲿ�ж�
	
	IT1 = 1; //ʹ�� INT1 �½����ж�
	EX1 = 1; //ʹ�� INT1 �ж�
	EA = 1;   //�����ж�

	LCDRW=0;        	//��ֻ�漰д��������ʽ�RW����ֱ���õ�
	UART1_Init();					//��ʼ������1
	LCD1602_Init();		//LCD1602��ʼ��
	LCD1602_Dis_Line(" Mode selection ",0);
	LCD1602_Dis_Line("--    MODE    --",1);
	List_Index = 1;
	printf("List_Index = %d\n",List_Index);
	delay(5);
	printf("List_Index = %d\n",List_Index);	
	Register_Threads();
	printf("List_Index = %d\n",List_Index);	
	
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
//�����ø÷�������ֵ�ṹ�壬�κα������汾̫�ϻᱨ��

/*
	�̵߳Ǽ�  	  ע�⣡���߳�4�����ã������������ͷ
*/
void Register_Threads(void)
{
	Thread tThread4 = {4, "--             --", Thread_four};
	Thread tThread2 = {2, "--  thread_2  --", Thread_two};
	Thread tThread3 = {3, "--  thread_3  --", Thread_three};
	Thread tThread1 = {1, "--  thread_1  --", Thread_one};	
	
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
	�̺߳�������ģ��
*/
void Thread_one(void)
{
	while(1)
	{
		LCD1602_Dis_Line("--  run  one  --",1);
		if(!List_Flag) return; //�˳����߳�	
	}
}

void Thread_two(void)
{
	while(1)
	{
		LCD1602_Dis_Line("--  run  two  --",1);
		if(!List_Flag) return; //�˳����߳�		
	}
}

void Thread_three(void)
{
	while(1)
	{
		LCD1602_Dis_Line("-- run three  --",1);
		if(!List_Flag) return; //�˳����߳�		
	}
}

void Thread_four(void) 
{
	while(1)
	{
		LCD1602_Dis_Line("-- run four  --",1);
		if(!List_Flag) return; //�˳����߳�		
	}
}


/********************************************************************/
//��ʱ����delay(u16 Count)�����β�Count,�޷���ֵ
/********************************************************************/
void delay(u16 Count)
{
  u8 i,j;
  while (Count--)//Count�βο�����ʱ����
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}
/********************************************************************/
//д��Һ��ģ����������ݺ���LCD1602_Write()�����β�cmdordata��
//writetype���޷���ֵ��
/********************************************************************/
void LCD1602_Write(u8 cmdordata,u8 writetype)
{
  if(writetype==0)		//�ж�д������
    LCDRS=0;					//д��������Ϣ
  else
    LCDRS=1;					//д��������Ϣ
  LCDDATA=cmdordata;	//�������߶˿�д����Ϣ
  delay(5);						//��ʱ�ȴ��ȶ�
  LCDEN=1;						//ģ��ʹ��
  delay(5);						//��ʱ�ȴ�д��
  LCDEN=0;						//ģ�鲻ʹ��
}
/********************************************************************/
//LCD1602��ʼ������LCD1602_init()�����βκͷ���ֵ
/********************************************************************/
void LCD1602_Init(void)
{
  LCD1602_Write(0x38,0);	//����16*2��ʾ��5*7����8λ���ݽӿ�
  LCD1602_Write(0x0C,0);	//���ÿ���ʾ������ʾ���
  LCD1602_Write(0x06,0);	//д�ַ����ַ�Զ���1
  LCD1602_Write(0x01,0);	//��ʾ��0������ָ����0
}
/********************************************************************/
//��ʾ�ַ�����LCD1602_Dis_Line(char *str, int y)��
//���β�: str Ҫ��ʾ���ַ���  yҪ��ʾ��Һ������һ�� 0��1  0��һ�� 1�ڶ���
//��ʾҺ���˵����������޷���ֵ
/********************************************************************/
void LCD1602_Dis_Line(char *str, int y)
{
  u8 i;									//�������ѭ������i
  if(!y)
  LCD1602_Write(0x80,0);//ѡ���һ��
  else LCD1602_Write(0xC0,0);  //�ڶ���
  delay(5);							//�ȴ�Һ����ʾ�ȶ�
  for(i=0;i<16;i++)			//��һ����д��15���ַ�
  {
		LCD1602_Write(str[i],1); 		//д��˵�������
		delay(5);						//�ȴ�Һ����ʾ�ȶ�
  }
}

/********************************************************************/
//����1��ʼ������UART1_Init()�����βΣ��޷���ֵ
/********************************************************************/
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
/********************************************************************/
//����1���͵��ַ����ݺ���U1SEND_C()�����β�SEND_C��Ϊ�����͵��ֽ�
//���ݣ��޷���ֵ
/********************************************************************/
void U1SEND_C(u8 SEND_C)
{
	TI=0;						//���������ɱ�־λ
	SBUF=SEND_C;		//��������
	while(!TI);			//�ȴ����ݷ������
}
/********************************************************************/
//�����ַ��ض�����char putchar(char ch)�����β�ch���з���ֵch
/********************************************************************/
char putchar(char ch)
{
	U1SEND_C((u8)ch);//ͨ������1��������
	return ch;
}


/* EC11 ��ť�жϺ��� */
void INT0_Isr(void) interrupt 0
{

	if(EC11_B==1)						//��EC11_B����Ϊ�ߵ�ƽ
		{
			if(List_Index<Max_Index)List_Index+=1;
			else  List_Index = Max_Index;
		}						//�ж�����������Ϊ
	else
		{
			if(List_Index > Min_Index)List_Index-=1;
			else  List_Index = Min_Index;
		}						//�ж�����������Ϊ
}

/* EC11 �����жϺ��� */
void INT1_Isr() interrupt 2
{
	unsigned int i = 0;
	
	if(EC11_C==0)						//�ڲ�����ȷ������
	{
		delay(20);
		if(EC11_C==0)
		{ 
			List_Flag = !List_Flag;				//�ж�Ϊ������Ϊ		    
		  	while(EC11_C==0);	//�ȴ������ڶ�������
		}
	}
}

