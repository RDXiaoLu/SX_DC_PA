#include "Bsp_lcd.h"

static void lcd_delay(unsigned int Cnt);
static void LCD_WRITE(unsigned char cmdordata,unsigned char writetype);
static void Dis_Line(unsigned char *str, int y);
//static bit Busy(void);


/* C99支持乱序赋值结构体
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//初始化LED外设结构体
struct lcd_operations LCD_fops = {
	 LCD_NUM,
	 LCD_WRITE,
	 Dis_Line,
};


/**
 * @brief  RELAY_CTL RELAY控制
 * @param  which-relay1 该板仅1个继电器  status 1-打开 0-关闭  
 * @retval 0 设置成功-1 设置失败
 */
 static void LCD_WRITE(unsigned char cmdordata,unsigned char writetype)
{
	LCDRS=writetype;					//判断写入类型0为命令1为数据
	lcd_delay(5);									//延时等待稳定
	LCDDATA&=0x0F;						//清高四位
	LCDDATA|=cmdordata&0xF0;	//写高四位
	LCDEN=1;lcd_delay(10);				//使能置“1”
	LCDEN=0;lcd_delay(10);				//使能清“0”
	cmdordata=cmdordata<<4;		//低四位移到高四位
	LCDDATA&=0x0F;						//清高四位
	LCDDATA|=cmdordata&0xF0;	//写低四位
	LCDEN=1;lcd_delay(10);				//使能置“1”
	LCDEN=0;lcd_delay(10);				//使能清“0”
}

/**
 * @brief  Dis_Line str-要显示的字符串 不超过16个 y显示的行 0-第一行 1-第二行
 * @param  无
 * @retval 无
 */
static void Dis_Line(unsigned char *str, int y)
{
  unsigned char i=0;									//定义控制循环变量i
  if(!y)
  	LCD_fops.write(0x80,0);//选择第一行
  else LCD_fops.write(0xC0,0);  //第二行
 	lcd_delay(100);							//等待液晶显示稳定
  for(i=0;i<16;i++)			//第一行先写入16个字符
  {

			lcd_delay(10);				//等待液晶显示稳定
			LCD_fops.write(str[i],1); 		//写入菜单项内容
			lcd_delay(10);				//等待液晶显示稳定 

  }
}

//static bit Busy(void)
//{
//	bit busy_flag = 0;
//	LCDRS = 0;
//	LCDRW = 1;
//	LCDEN = 1;
//	lcd_delay(10);
//	busy_flag = (bit)(P0 & 0x80);
//	LCDEN = 0;
//	return busy_flag;
//}

/**
 * @brief  LCD_Init LCD初始化
 * @param  无
 * @retval 无
 */
void LCD_Init(void)
{
	//配置P0.0-2为准双向/弱上拉模式
	P0M0&=0xF8;			//P0M0.4-6=0
	P0M1&=0xF8;			//P0M1.4-6=0
	//配置P2.4-7为准双向/弱上拉模式
	P2M0&=0x0F;			//P2M0.4-7=0
	P2M1&=0x0F;			//P2M1.4-7=0
	lcd_delay(5);				//等待I/O模式配置稳定
	LCDRW=0;				//因只涉及写入操作，故将RW引脚直接置低
	lcd_delay(5);				//等待I/O模式配置稳定
	LCD_fops.write(0x32,0);lcd_delay(10);//0x32非命令，参见HD44780数据手册
	LCD_fops.write(0x28,0);lcd_delay(10);//数据总线为4位，显示2行，5*7点阵/每字符
	LCD_fops.write(0x0C,0);lcd_delay(10);//设置开显示，不显示光标
	LCD_fops.write(0x06,0);lcd_delay(10);//写入新数据后显示屏整体不移动仅光标右移
	LCD_fops.write(0x01,0);lcd_delay(10);//写入清屏命令

}
/**
 * @brief  LCD_Init LCD延时
 * @param  无
 * @retval 无
 */
static void lcd_delay(unsigned int Cnt)
{
  unsigned char i,j;
  while (Cnt--)//Count形参控制延时次数
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}