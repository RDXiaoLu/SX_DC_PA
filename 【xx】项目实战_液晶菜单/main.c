/*********************************************************************
 * 实验名称：	EC11+1602液晶简易多级菜单实验
 * 选配书籍：	上册-《深入浅出STC8增强型51单片机进阶攻略》2022已出版
							下册-《深入浅出STC8增强型51单片机实战攻略》还在写作
 * 书籍作者：	龙顺宇编著 清华大学出版社出版
 * 淘宝店铺：	https://520mcu.taobao.com/
 * 实验平台：	思修电子STC8“战将”系列小王子开发板
 * 芯片型号：	STC8H8K64U-LQFP64（程序微调后可移植至STC其他系列单片机）
 * 时钟说明：	片内IRC高速24MHz时钟，使用STC-ISP下载程序时配置
 * 实验说明：	先断电后操作，连接07-EB、06-EA、05-EC短路帽，插装1602字符
 液晶模块（注意1脚位置，切莫插反或插错），检查无误后给开发板上电，做好ST
 C-ISP配置并下载程序。
 液晶第一行显示：
 液晶第二行显示：
 EC11左右旋转选择要执行的功能（线程）
 EC11单击后进入该线程，此时页面显示执行线程的线程函数名称，并且执行线程函数。再次单机EC11。返回选择页面。
 注意： EC11_B 接到 P3^2引脚！！！   EC11_C 接到 P3^3引脚 
*********************************************************************/
#include	"STC8H.h"	//主控芯片的头文件
#include    "stdio.h"
#include    "stdlib.h"
/***************************常用数据类型定义*************************/
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
typedef unsigned char  uint8_t;
typedef unsigned int   uint16_t;
typedef unsigned long  uint32_t;
/***************************端口/引脚定义区域************************/
sbit EC11_B=P0^7; 	//EC11的B引脚（脉冲）
sbit EC11_A=P3^2; 	//EC11的A引脚（脉冲）		  EC11_B 接到 P3^2引脚 启用INT0外部下降沿中断
sbit EC11_C=P3^3; 	//EC11的E引脚（内部按键）	 启用INT1外部下降沿中断
sbit LCDRS=P7^4;		//LCD1602数据/命令选择端口
sbit LCDEN=P7^6;		//LCD1602使能信号端口
sbit LCDRW=P7^5;    //LCD1602读写控制端口
#define LCDDATA  P2	//LCD1602数据端口DB0至DB7
/***************************用户自定义数据区域***********************/
#define Max_Index 3 
#define Min_Index 1
//#define NULL (void *)0

int  List_Index = 2; 	//液晶主菜单索引
int  List_Flag = 0;	//索引标志 1则可以进入选项 0则只停留在菜单选择

/* 定义线程结构体 */
typedef struct Thread{
	int PID;   					//进程号
	char name[17];				//显示内容
	void (*Thread_Fun)(void);	//线程函数
	struct Thread *Next;
}Thread,*PThread;

PThread g_pThread = NULL; //链表头节点

/*****************************函数声明区域***************************/
void delay(u16 Count);			//延时函数
void LCD1602_Write(u8 cmdordata,u8 writetype);
//写入液晶模组命令或数据函数
void LCD1602_Init(void);       //液晶初始化函数
void LCD1602_Dis_Line(char *str, int y);		//1602行显示函数

void OS_Init(void);   			          //液晶显示菜单框架初始化
void Register_Threads(void);              //线程登记函数
void Thread_List(PThread ptThread);       //线程链表函数
PThread Thread_Index(int Index);          //线程索引函数

void Thread_one(void);				//线程1函数
void Thread_two(void);				//线程2函数
void Thread_three(void);			//线程3函数
void Thread_four(void);				//线程4函数
/***************************调试接口***********************/
#define SYSCLK 24000000UL		//系统时钟频率值
#define BAUD_SET  (65536-SYSCLK/9600/4)//波特率设定与计算 
/*****************************函数声明区域***************************/								   
void UART1_Init(void);			//串口1初始化函数
void U1SEND_C(u8 SEND_C);		//串口1发送单字符数据函数
char putchar(char ch);			//发送字符重定向函数
/******************************主函数区域****************************/
void main(void)
{
	PThread ptThread = NULL;

	OS_Init();


	printf("--------------------------------------\r\n");
	printf("【UART1】:MOD1 24MHz 9600bps\r\n");	
	printf("【UART1】:RXD/P3.0, TXD/P3.1\r\n");	
	printf("--------------------------------------\r\n");

	while(1)
	{
		
		printf("List_Index = %d\n",List_Index);	
		ptThread = 	Thread_Index(List_Index);	

		
		if(List_Flag)	    //EC11按下 执行当前选择线程的线程函数
		{
			ptThread->Thread_Fun();
			List_Index = 1;  //退出线程函数后 从线程1开始选择
		}
		
		LCD1602_Dis_Line(ptThread->name,1); //刷新显示
	}	
}

void OS_Init(void)
{
	//配置P7.4-6为准双向/弱上拉模式
	P7M0&=0x8F;				//P7M0.4-6=0
	P7M1&=0x8F;				//P7M1.4-6=0
	//配置P2为准双向/弱上拉模式
	P2M0=0x00;				//P2M0.0-7=0
	P2M1=0x00;				//P2M1.0-7=0
	//配置P0.5-7为准双向/弱上拉模式
	P0M0&=0x1F;				//P0M0.5-7=0
	P0M1&=0x1F;				//P0M1.5-7=0
	delay(5);					//等待I/O模式配置稳定
	//使能P3^2 P3^3的外部中断
	
	IT1 = 1; //使能 INT1 下降沿中断
	EX1 = 1; //使能 INT1 中断
	EA = 1;   //打开总中断

	LCDRW=0;        	//因只涉及写入操作，故将RW引脚直接置低
	UART1_Init();					//初始化串口1
	LCD1602_Init();		//LCD1602初始化
	LCD1602_Dis_Line(" Mode selection ",0);
	LCD1602_Dis_Line("--    MODE    --",1);
	List_Index = 1;
	printf("List_Index = %d\n",List_Index);
	delay(5);
	printf("List_Index = %d\n",List_Index);	
	Register_Threads();
	printf("List_Index = %d\n",List_Index);	
	
	IT0 = 1;  //使能INT0下降沿中断
	EX0 = 1;  //使能INT0中断

}
/*
	线程索引
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
	线程初始化 
struct Thread tThread1 = {
//		.PID = 1,
//		.name = "--  thread_1x  --",
//		.Thread = Thread_one,
//};
//可以用该方法乱序赋值结构体，奈何编译器版本太老会报错

/*
	线程登记  	  注意！！线程4不可用，用来填充链表头
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
	线程链表建立 头插法
*/
void Thread_List(PThread ptThread)
{
	ptThread->Next = g_pThread;
	g_pThread = ptThread;
}
/*
	线程函数建立模板
*/
void Thread_one(void)
{
	while(1)
	{
		LCD1602_Dis_Line("--  run  one  --",1);
		if(!List_Flag) return; //退出该线程	
	}
}

void Thread_two(void)
{
	while(1)
	{
		LCD1602_Dis_Line("--  run  two  --",1);
		if(!List_Flag) return; //退出该线程		
	}
}

void Thread_three(void)
{
	while(1)
	{
		LCD1602_Dis_Line("-- run three  --",1);
		if(!List_Flag) return; //退出该线程		
	}
}

void Thread_four(void) 
{
	while(1)
	{
		LCD1602_Dis_Line("-- run four  --",1);
		if(!List_Flag) return; //退出该线程		
	}
}


/********************************************************************/
//延时函数delay(u16 Count)，有形参Count,无返回值
/********************************************************************/
void delay(u16 Count)
{
  u8 i,j;
  while (Count--)//Count形参控制延时次数
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}
/********************************************************************/
//写入液晶模组命令或数据函数LCD1602_Write()，有形参cmdordata和
//writetype，无返回值。
/********************************************************************/
void LCD1602_Write(u8 cmdordata,u8 writetype)
{
  if(writetype==0)		//判断写入类型
    LCDRS=0;					//写入命令信息
  else
    LCDRS=1;					//写入数据信息
  LCDDATA=cmdordata;	//向数据线端口写入信息
  delay(5);						//延时等待稳定
  LCDEN=1;						//模块使能
  delay(5);						//延时等待写入
  LCDEN=0;						//模块不使能
}
/********************************************************************/
//LCD1602初始化函数LCD1602_init()，无形参和返回值
/********************************************************************/
void LCD1602_Init(void)
{
  LCD1602_Write(0x38,0);	//配置16*2显示，5*7点阵，8位数据接口
  LCD1602_Write(0x0C,0);	//设置开显示，不显示光标
  LCD1602_Write(0x06,0);	//写字符后地址自动加1
  LCD1602_Write(0x01,0);	//显示清0，数据指针清0
}
/********************************************************************/
//显示字符函数LCD1602_Dis_Line(char *str, int y)，
//有形参: str 要显示的字符串  y要显示在液晶的哪一行 0或1  0第一行 1第二行
//表示液晶菜单的索引，无返回值
/********************************************************************/
void LCD1602_Dis_Line(char *str, int y)
{
  u8 i;									//定义控制循环变量i
  if(!y)
  LCD1602_Write(0x80,0);//选择第一行
  else LCD1602_Write(0xC0,0);  //第二行
  delay(5);							//等待液晶显示稳定
  for(i=0;i<16;i++)			//第一行先写入15个字符
  {
		LCD1602_Write(str[i],1); 		//写入菜单项内容
		delay(5);						//等待液晶显示稳定
  }
}

/********************************************************************/
//串口1初始化函数UART1_Init()，无形参，无返回值
/********************************************************************/
void UART1_Init(void)
{
	SCON=0x50;			//181结构，可变波特率，允许数据接收
	AUXR|=0x01;			//串口1选择定时器2为波特率发生器
	AUXR|=0x04;			//定时器时钟1T模式
	T2L=BAUD_SET;		//设置定时初始值
	T2H=BAUD_SET>>8;//设置定时初始值
	AUXR|=0x10;			//定时器2开始计时
	RI=0;TI=0;			//清除接收数据标志位和发送数据标志位
}
/********************************************************************/
//串口1发送单字符数据函数U1SEND_C()，有形参SEND_C即为欲发送单字节
//数据，无返回值
/********************************************************************/
void U1SEND_C(u8 SEND_C)
{
	TI=0;						//清除发送完成标志位
	SBUF=SEND_C;		//发送数据
	while(!TI);			//等待数据发送完成
}
/********************************************************************/
//发送字符重定向函数char putchar(char ch)，有形参ch，有返回值ch
/********************************************************************/
char putchar(char ch)
{
	U1SEND_C((u8)ch);//通过串口1发送数据
	return ch;
}


/* EC11 旋钮中断函数 */
void INT0_Isr(void) interrupt 0
{

	if(EC11_B==1)						//若EC11_B引脚为高电平
		{
			if(List_Index<Max_Index)List_Index+=1;
			else  List_Index = Max_Index;
		}						//判定“右旋”行为
	else
		{
			if(List_Index > Min_Index)List_Index-=1;
			else  List_Index = Min_Index;
		}						//判定“左旋”行为
}

/* EC11 按键中断函数 */
void INT1_Isr() interrupt 2
{
	unsigned int i = 0;
	
	if(EC11_C==0)						//内部按键确定按下
	{
		delay(20);
		if(EC11_C==0)
		{ 
			List_Flag = !List_Flag;				//判定为单击行为		    
		  	while(EC11_C==0);	//等待按键第二次松手
		}
	}
}

