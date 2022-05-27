#include "Bsp_lap.h"
#include "intrins.h"


static char IapRead(int addr); 
static void IapProgram(int addr, char dat);
static void IapErase(int addr);
static void Iap_Int_Program(int addr, int dat); 
static int Iap_Int_Read(int addr);
static void Iap_Int_Erase(int addr); 
/* C99支持乱序赋值结构体
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//初始化LED外设结构体
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
 * @brief  IapIdle Iap控制
 * @param  which 1-4  D1 D2 D3 D4	 status 1-亮 0-灭
 * @retval 无
 */
static void IapIdle(void) 
{ 
	IAP_CONTR = 0; //关闭 IAP 功能
	IAP_CMD = 0; //清除命令寄存器
	IAP_TRIG = 0; //清除触发寄存器
	IAP_ADDRH = 0x80; //将地址设置到非 IAP 区域
	IAP_ADDRL = 0; 
}

/**
 * @brief  IapRead Iap读取
 * @param  addr-读取地址
 * @retval 无
 */
static char IapRead(int addr) 
{ 
 	 char dat; 
 
	 IAP_CONTR = 0x80; //使能 IAP
	 IAP_TPS = 24; //设置擦除等待参数 24MHz
	 IAP_CMD = 1; //设置 IAP 读命令
	 IAP_ADDRL = addr; //设置 IAP 低地址
	 IAP_ADDRH = addr >> 8; //设置 IAP 高地址
	 IAP_TRIG = 0x5a; //写触发命令(0x5a)
	 IAP_TRIG = 0xa5; //写触发命令(0xa5)
	 _nop_(); 
	 dat = IAP_DATA; //读 IAP 数据
	 IapIdle(); //关闭 IAP 功能
 	 return dat; 
}

/**
 * @brief  IapErase Iap写入
 * @param  addr-写入地址  dat-写入数据
 * @retval 无
 */
static void IapProgram(int addr, char dat) 
{ 
	 IAP_CONTR = 0x80; //使能 IAP
	 IAP_TPS = 24; //设置擦除等待参数 24MHz
	 IAP_CMD = 2; //设置 IAP 写命令
	 IAP_ADDRL = addr; //设置 IAP 低地址
	 IAP_ADDRH = addr >> 8; //设置 IAP 高地址
	 IAP_DATA = dat; //写 IAP 数据
	 IAP_TRIG = 0x5a; //写触发命令(0x5a)
	 IAP_TRIG = 0xa5; //写触发命令(0xa5)
	 _nop_(); 
	 IapIdle(); //关闭 IAP 功能
}			
/**
 * @brief  IapErase Iap擦除
 * @param  无
 * @retval 无
 */

static void IapErase(int addr) 
{ 
	 IAP_CONTR = 0x80; //使能 IAP
	 IAP_TPS = 24; //设置擦除等待参数 24MHz
	 IAP_CMD = 3; //设置 IAP 擦除命令
	 IAP_ADDRL = addr; //设置 IAP 低地址
	 IAP_ADDRH = addr >> 8; //设置 IAP 高地址
	 IAP_TRIG = 0x5a; //写触发命令(0x5a)
	 IAP_TRIG = 0xa5; //写触发命令(0xa5)
	 _nop_(); //
	 IapIdle(); //关闭 IAP 功能
}

/**
 * @brief  Iap_Int_Program Iap写入整形
 * @param  dat-写入int类型数据   addr
 * @retval 无
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
 * @brief  Iap_Int_Erase Iap擦出整形
 * @param  addr-擦除地址
 * @retval 无
 */
static void Iap_Int_Erase(int addr) 
{
	IapErase(addr);
	IapErase(addr+1); 
	IapErase(addr+2);
	IapErase(addr+3);
}
