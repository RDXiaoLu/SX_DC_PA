/*********************************************************************
 * 实验名称：	直流保护模块究极实验
 
 灯+	         按键+	           继电器+
 
 电压采集+	     1602+	           内部EEPROM+
 
 链表线程+	     过流定时恢复+     过功率定时恢复+			
 
 欠压检测+		 过压检测+			过流检测+
				  
 过功率检测
							
 * 选配书籍：	上册-《深入浅出STC8增强型51单片机进阶攻略》2022已出版
							下册-《深入浅出STC8增强型51单片机实战攻略》还在写作
 * 书籍作者：	龙顺宇编著 清华大学出版社出版
 * 淘宝店铺：	https://520mcu.taobao.com/
 * 实验平台：	思修电子STC8“战将”系列小王子开发板
 * 芯片型号：	STC8H3K64S4-LQFP32（程序微调后可移植至STC其他系列单片机）
 * 时钟说明：	片内IRC高速24MHz时钟，使用STC-ISP下载程序时配置
 * 实验说明：	插装1602字符液晶模块（注意1脚位置，切莫插反或插错），检查无误后给开发板上电，做好ST
 C-ISP配置并下载程序。
 线程1配置电压保护范围，默认直接存储到内置IAP，可通过配置宏定义，选择是否启用EEPOM中的参数，或将参数存储到EEPROM中
 线程2配置电流保护范围，默认直接存储到内置IAP，可通过配置宏定义，选择是否启用EEPOM中的参数，或将参数存储到EEPROM中
 线程3配置功率保护范围，默认直接存储到内置IAP，可通过配置宏定义，选择是否启用EEPOM中的参数，或将参数存储到EEPROM中
 线程4，显示，电流+电压+功率，并不断比较电流，电压，功率是否符合设定参数，如果不符合，则打开继电器并启用自恢复定时器，打开相应状态指示灯。
 
 KEY1左右旋转选择要执行的功能（线程）
 KEY2单击后进入该线程，此时页面显示执行线程的线程函数名称，并且执行线程函数。再次单机KEY2。返回选择页面。
 
*********************************************************************/
#include "STC8H.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"  //用到pow 
#include "./led/Bsp_led.h"
#include "./key/Bsp_key.h"
#include "./lcd/Bsp_lcd.h"
#include "./adc/Bsp_adc.h"
#include "./uart/Bsp_uart.h"
#include "./relay/Bsp_relay.h"
#include "./lap/Bsp_lap.h"


/***************************用户自定义数据区域***********************/
#define OPEN 1
#define CLOSE 0

#define Max_Index 4 
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
unsigned char g_flag = 0;  //线程中断使能标志
#define OPEN 1
#define CLOSE 0

int V_MAX =  35;	  /* 输入限定范围最大值 */
int V_MIN =  25;	  /* 输入限定范围最小值 */
static unsigned int AD_Vtemp[10]={0};//装载10次ADC采样数据
static unsigned int AD_val=0;				
char v_maxstr[4];	  //初始化
char v_minstr[4];
char vol_str[15];


int I_MAX =  10;	  /* 输入限定范围最大值 */
int I_MIN =  0;	  /* 输入限定范围最小值 */			
char i_maxstr[4];	  //初始化
char i_minstr[4];
char cur_str[15];

int P_MAX =  30;	  /* 输入限定范围最大值 */
int P_MIN =  0;	  /* 输入限定范围最小值 */			
char p_maxstr[4];	  //初始化
char p_minstr[4];
char pow_str[10];

double GETcurrent = 0.0;
double GETvoltage = 0.0;
double Power = 0.0;

#define Measure_I 0x00
#define Measure_V 0x02
/*****************************函数声明区域***************************/
void delay(unsigned int Count);			//延时函数


void OS_Init(void);   			          //液晶显示菜单框架初始化
void Register_Threads(void);              //线程登记函数
void Thread_List(PThread ptThread);       //线程链表函数
PThread Thread_Index(int Index);          //线程索引函数

void Thread_one(void);				//线程1函数
void Thread_two(void);				//线程2函数
void Thread_three(void);			//线程3函数
void Thread_four(void);				//线程4函数
void Thread_five(void);				//线程5函数

extern void delay(unsigned int Count); /* 粗略延时 */
static void OS_Init(void);	 /* 系统硬件初始化 */
void Timer2Init(void); /* 自恢复定时器 */

/**
 * @brief  AVG_AD_Vtemp() 输入电压中位数平均值滤波函数
 * @param  无
 * @retval 无
 */
void AVG_AD_Vtemp(void)
{
	unsigned char i,j;								//定义排序用内外层循环变量i和j
	unsigned int temp;							//定义中间“暂存”变量temp
	for(i=10;i>=1;i--)		//外层循环
	{
		for(j=0;j<(i-1);j++)//内层循环
		{
			if(AD_Vtemp[j]>AD_Vtemp[j+1])//数值比较
			{
				temp=AD_Vtemp[j];//数值换位
				AD_Vtemp[j]=AD_Vtemp[j+1];
				AD_Vtemp[j+1]=temp;
			}
		}
	}
	for(i=2;i<=7;i++)				//去掉2个最低去掉2个最高
	AD_val+=AD_Vtemp[i];		//将中间6个数值累加
	AD_val=(unsigned int)(AD_val/6);	//累加和求平均值
}

/******************************主函数区域****************************/
void main(void)
{
	PThread ptThread = NULL;  /* 初始化链表 */

	OS_Init(); /* 系统硬件初始化 */

	printf("--------------------------------------\r\n");
	printf("欢迎来到思修电子直流保护开发板的终极实验");	
	printf("--------------------------------------\r\n");

	while(1)
	{
		/* 根据线程索引执行相应函数 */
		printf("List_Index = %d\n",List_Index);	
		ptThread = 	Thread_Index(List_Index);	

		
		if(List_Flag)	    //EC11按下 执行当前选择线程的线程函数
		{
			ptThread->Thread_Fun();
			List_Index = 1;  //退出线程函数后 从线程1开始选择
		}
		
		LCD_fops.dis_line(ptThread->name,1); //刷新显示
		LCD_fops.dis_line("                ",2);
	}	
}

/**
 * @brief  OS_Init 硬件初始化
 * @param  无
 * @retval 无
 */
void OS_Init(void)
{
	LED_Init();	
	KEY_Init();
	RELAY_Init();
	LCD_Init();
	ADC_Init();
	UART1_Init();
	Timer2Init();		//2毫秒@24.000MHz
	//使能P3^2 P3^3的外部中断
	
	IT1 = 1; //使能 INT1 下降沿中断
	EX1 = 1; //使能 INT1 中断
	EA = 1;   //打开总中断

	LCD_fops.dis_line(" Mode selection ",0);
	LCD_fops.dis_line("--    MODE    --",1);
	List_Index = 1;
	delay(5);
	Register_Threads();
	
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
//可以用该方法乱序赋值结构体，C99标准

/*
	线程登记  	  注意！！线程5不可用，用来填充链表头
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
	线程链表建立 头插法
*/
void Thread_List(PThread ptThread)
{
	ptThread->Next = g_pThread;
	g_pThread = ptThread;
}
/*
	
*/
/**
 * @brief  Thread_one 线程函数建立模板
 * @param  无
 * @retval 无
 */
void Thread_one(void)
{
	 unsigned char index = 0;
	 LCD_fops.dis_line("-protect V set -",1);
	 LCD_fops.write(0x0d,0);   //打开所在字符闪烁
	 sprintf(v_maxstr, "%2d",V_MAX);  		  /* 变量转换成字符串存放在对应字符数组中 */
	sprintf(v_minstr, "%2d",V_MIN); 
	 /* 液晶显示刷新 */
	LCD_fops.dis_str(0,2,v_minstr);
	LCD_fops.dis_char(3,2,'<');
	LCD_fops.dis_str(4,2,"V__range");	//在x=4 y=2 的位置开始显示字符串vol_str	
	LCD_fops.dis_char(12,2,'<');
	LCD_fops.dis_str(14,2,v_maxstr);
	 while(1)
	 {
		if(KEY_fops.read(7) == 0)		//光标选择索引控制
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
			if(KEY_fops.read(3) == 0)	 //相应动作判断
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
			if(KEY_fops.read(4) == 0)	 //相应动作判断
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
		/* 将设置参数存储到内部EEPROM中 */
#if		1
		IAP_fops.erase(Region_1);
		IAP_fops.program(Region_1,V_MIN);
		IAP_fops.erase(Region_2);
		IAP_fops.program(Region_2,V_MAX);
#endif
		sprintf(v_maxstr, "%2d",V_MAX);  		  /* 变量转换成字符串存放在对应字符数组中 */
		sprintf(v_minstr, "%2d",V_MIN); 
			
		switch(index)
		{
			 case 0: LCD_fops.dis_char(0,2,v_minstr[0]);break; //V_MAX	V_MIN
			 case 1: LCD_fops.dis_char(1,2,v_minstr[1]);break;
			 case 2: LCD_fops.dis_char(14,2,v_maxstr[0]);break;
			 case 3: LCD_fops.dis_char(15,2,v_maxstr[1]);break;
			 default: LCD_fops.dis_char(0,2,v_minstr[0]);break;
		}
		
	 	if(!List_Flag) {LCD_fops.write(0x0c,0); return;} //退出该线程	
	 }
}

void Thread_two(void)
{
	unsigned char index = 0;
	 LCD_fops.dis_line("-protect I set -",1);
	 LCD_fops.write(0x0d,0);   //打开所在字符闪烁
	 sprintf(i_maxstr, "%2d",I_MAX);  		  /* 变量转换成字符串存放在对应字符数组中 */
	sprintf(i_minstr, "%2d",I_MIN); 
	 /* 液晶显示刷新 */
	LCD_fops.dis_str(0,2,i_minstr);
	LCD_fops.dis_char(3,2,'<');
	LCD_fops.dis_str(4,2,"I__range");	//在x=4 y=2 的位置开始显示字符串vol_str	
	LCD_fops.dis_char(12,2,'<');
	LCD_fops.dis_str(14,2,i_maxstr);
	 while(1)
	 {
		if(KEY_fops.read(7) == 0)		//光标选择索引控制
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
			if(KEY_fops.read(3) == 0)	 //相应动作判断
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
			if(KEY_fops.read(4) == 0)	 //相应动作判断
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
		/* 将设置参数存储到内部EEPROM中 */
#if		1
		IAP_fops.erase(Region_3);
		IAP_fops.program(Region_3,I_MIN);
		IAP_fops.erase(Region_4);
		IAP_fops.program(Region_4,I_MAX);
#endif
		sprintf(i_maxstr, "%2d",I_MAX);  		  /* 变量转换成字符串存放在对应字符数组中 */
		sprintf(i_minstr, "%2d",I_MIN); 
			
		switch(index)
		{
			 case 0: LCD_fops.dis_char(0,2,i_minstr[0]);break; //V_MAX	V_MIN
			 case 1: LCD_fops.dis_char(1,2,i_minstr[1]);break;
			 case 2: LCD_fops.dis_char(14,2,i_maxstr[0]);break;
			 case 3: LCD_fops.dis_char(15,2,i_maxstr[1]);break;
			 default: LCD_fops.dis_char(0,2,i_minstr[0]);break;
		}
		
	 	if(!List_Flag) {LCD_fops.write(0x0c,0); return;} //退出该线程	
	 }
}

void Thread_three(void)
{
	unsigned char index = 0;
	 LCD_fops.dis_line("-protect P set -",1);
	 LCD_fops.write(0x0d,0);   //打开所在字符闪烁
	 sprintf(p_maxstr, "%2d",P_MAX);  		  /* 变量转换成字符串存放在对应字符数组中 */
	sprintf(p_minstr, "%2d",P_MIN); 
	 /* 液晶显示刷新 */
	LCD_fops.dis_str(0,2,p_minstr);
	LCD_fops.dis_char(3,2,'<');
	LCD_fops.dis_str(4,2,"P__range");	//在x=4 y=2 的位置开始显示字符串vol_str	
	LCD_fops.dis_char(12,2,'<');
	LCD_fops.dis_str(14,2,p_maxstr);
	 while(1)
	 {
		if(KEY_fops.read(7) == 0)		//光标选择索引控制
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
			if(KEY_fops.read(3) == 0)	 //相应动作判断
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
			if(KEY_fops.read(4) == 0)	 //相应动作判断
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
		/* 将设置参数存储到内部EEPROM中 */
#if		1
		IAP_fops.erase(Region_5);
		IAP_fops.program(Region_5,I_MIN);
		IAP_fops.erase(Region_6);
		IAP_fops.program(Region_6,I_MAX);
#endif
		sprintf(i_maxstr, "%2d",P_MAX);  		  /* 变量转换成字符串存放在对应字符数组中 */
		sprintf(i_minstr, "%2d",P_MIN); 
			
		switch(index)
		{
			 case 0: LCD_fops.dis_char(0,2,p_minstr[0]);break; //V_MAX	V_MIN
			 case 1: LCD_fops.dis_char(1,2,p_minstr[1]);break;
			 case 2: LCD_fops.dis_char(14,2,p_maxstr[0]);break;
			 case 3: LCD_fops.dis_char(15,2,p_maxstr[1]);break;
			 default: LCD_fops.dis_char(0,2,p_minstr[0]);break;
		}
		
	 	if(!List_Flag) {LCD_fops.write(0x0c,0); return;} //退出该线程	
	 }
}

void Thread_four(void) 
{
	bit flag;
	unsigned char i;
	LCD_fops.dis_line("-Display param -",1);

	/* 是否使用IAP中存储的参数 第一次使用需要用其他线程设置参数 */
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

			GETcurrent=(double)(AD_val*0.060791015625/111*5);	//按照2.5V/4096计算得到的结果
			//存在误差 利用matlab进行曲线拟合
			GETcurrent = -0.0062*pow(GETcurrent,6.0) + 0.0776*pow(GETcurrent,5.0) - 0.3777*pow(GETcurrent,4.0)+0.8987*pow(GETcurrent,3.0) -1.0724*pow(GETcurrent,2.0) + 1.4202* GETcurrent + 0.0714;
			AD_val = 0;

			//sprintf(cur_str, "%.1f",GETcurrent-(double)(((int)(GETcurrent*100))%10)/100);   //保留1位小数，不四舍五入进位
			sprintf(cur_str, "%.2f",GETcurrent);
			GETcurrent = atof(cur_str);	//字符串转换成double类型
			if(I_MAX >= GETcurrent && I_MIN <= GETcurrent)
			{
				RELAY_fops.relay_ctl(1,OPEN);	 /* 继电器控制 */
				LED_fops.led_ctl(2,CLOSE);
				AUXR &= 0xEF;  	//不使能定时器2
				AUXR |= 0x10;  	//使能定时器2
				IE2 &= 0xFB;  //关闭定时器2中断
			}
			else if(I_MAX < GETcurrent || I_MIN > GETcurrent)  //不满足电流条件
			{
				LED_fops.led_ctl(2,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* 继电器控制 */
				AUXR |= 0x10;  	//使能定时器2
				IE2 = 0x04;  //开启定时器2中断
			}
			LCD_fops.dis_str(0,1,"I:");
			LCD_fops.dis_str(2,1,cur_str);	//在x=4 y=2 的位置开始显示字符串vol_str	   
			LCD_fops.dis_str(6,1,"A "); 
			//printf("out Cur: %.1f\r\n",GETcurrent);	
		}
		else{
			for(i=0; i<10; i++)
			{
				AD_Vtemp[i] = ADC_fops.adc_get(Measure_V);		
			}
			AVG_AD_Vtemp();
	
			GETvoltage=(double)(AD_val*0.0006103515625*40);	//按照5V/4096计算得到的结果
			AD_val = 0;
			if(V_MAX >= GETvoltage && V_MIN <= GETvoltage)
			{
				RELAY_fops.relay_ctl(1,OPEN);	 /* 继电器控制 */
				LED_fops.led_ctl(3,CLOSE);       
				LED_fops.led_ctl(4,CLOSE);
			}
			else if(V_MAX < GETvoltage )  //过压
			{
				LED_fops.led_ctl(3,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* 继电器控制 */
			}
			else if(V_MIN > GETvoltage )  //欠压
			{
				LED_fops.led_ctl(4,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* 继电器控制 */
			}
			//memset(AD_Vtemp, 0, sizeof(AD_Vtemp));
			sprintf(vol_str, "%.2f",GETvoltage);   //得到电压 乘以 放大倍数40
			GETvoltage = atof(vol_str);	//字符串转换成double类型
			LCD_fops.dis_str(8,1,"V:");
			LCD_fops.dis_str(10,1,vol_str);	//在x=8 y=8 的位置开始显示字符串vol_str	   
			LCD_fops.dis_char(15,1,'V'); 	
			//printf("out Vol: %.2f\r\n",GETvoltage);
		}
		Power = GETvoltage * GETcurrent;
		if(P_MAX >= Power && P_MIN <= Power)
			{
				RELAY_fops.relay_ctl(1,OPEN);	 /* 继电器控制 */
				LED_fops.led_ctl(1,CLOSE);
				AUXR &= 0xEF;  	//不使能定时器2
				AUXR |= 0x10;  	//使能定时器2
				IE2 &= 0xFB;  //关闭定时器2中断
			}
			else if(P_MAX < GETcurrent || P_MIN > GETcurrent)  //不满足功率条件
			{
				LED_fops.led_ctl(1,OPEN);
				RELAY_fops.relay_ctl(1,CLOSE);	   /* 继电器控制 */
				AUXR |= 0x10;  	//使能定时器2
				IE2 = 0x04;  //开启定时器2中断
			}
		sprintf(pow_str, "%.2f",Power);   //得到功率
		LCD_fops.dis_str(0,2,"P:");
		LCD_fops.dis_str(2,2,pow_str);
		LCD_fops.dis_char(7,2,'W');
		flag = !flag;
		if(!List_Flag) return; //退出该线程		
	}
}

void Thread_five(void) 
{
	while(1)
	{
		LCD_fops.dis_line("-- run four  --",1);
		if(!List_Flag) return; //退出该线程		
	}
}


/********************************************************************/
//延时函数delay(u16 Count)，有形参Count,无返回值
/********************************************************************/
void delay(unsigned int Count)
{
  unsigned char i,j;
  while (Count--)//Count形参控制延时次数
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}


/* KEY1 旋钮中断函数 */
void INT0_Isr(void) interrupt 0
{

	if(KEY_fops.read(1)==0)						//索引确定是否
	{	 
		 if(List_Index<Max_Index)List_Index+=1;
		 else  List_Index = Min_Index;
		 while(!KEY_fops.read(1));
	}					

}

/**
 * @brief  Timer2Init 定时器2初始化
 * @param  无
 * @retval 无
 */
void Timer2Init(void)		//2毫秒@24.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x80;		//设置定时初值
	T2H = 0x44;		//设置定时初值	
}




/* EC11 按键中断函数 */
void INT1_Isr() interrupt 2
{
	
	if(KEY_fops.read(2)==0)						//索引确定是否
	{
		 List_Flag = !List_Flag;				//判定为单击行为
	}
	while(!KEY_fops.read(2));
}

/**
 * @brief  TM2_Isr 定时器2 自恢复中断
 * @param  无
 * @retval 无
 */
void TM2_Isr() interrupt 12 
{
	static unsigned int i = 0;
	if(i>=4000)	  //大约8秒自恢复一次
	{
		RELAY_fops.relay_ctl(1,OPEN); //自恢复
		i = 0;
	    AUXR &= 0xEF;  	//不使能定时器2
	}
	i++;
}