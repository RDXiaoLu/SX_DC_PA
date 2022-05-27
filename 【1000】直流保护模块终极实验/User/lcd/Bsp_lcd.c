#include "Bsp_lcd.h"

static void lcd_delay(unsigned int Cnt);
static void LCD_WRITE(unsigned char cmdordata,unsigned char writetype);
static void Dis_Line(unsigned char *str, int y);
static void Dis_Char(int x, int y, char c);
static void Dis_Str(int x, int y, char *str);
//static bit Busy(void);


/* C99֧������ֵ�ṹ��
struct led_operations LED{
	.num = LED_NUM;
	.led_ctl = LED_CTL;	
};
*/

//��ʼ��LED����ṹ��
struct lcd_operations LCD_fops = {
	 LCD_NUM,
	 LCD_WRITE,
	 Dis_Line,
	 Dis_Char,
	 Dis_Str,
};


/**
 * @brief  LCD_WRITE LCDд����/�����
 * @param  cmdordata-д�����   writetype-д������
 * @retval ��
 */
 static void LCD_WRITE(unsigned char cmdordata,unsigned char writetype)
{
	LCDRS=writetype;					//�ж�д������0Ϊ����1Ϊ����
	lcd_delay(5);									//��ʱ�ȴ��ȶ�
	LCDDATA&=0x0F;						//�����λ
	LCDDATA|=cmdordata&0xF0;	//д����λ
	LCDEN=1;lcd_delay(10);				//ʹ���á�1��
	LCDEN=0;lcd_delay(10);				//ʹ���塰0��
	cmdordata=cmdordata<<4;		//����λ�Ƶ�����λ
	LCDDATA&=0x0F;						//�����λ
	LCDDATA|=cmdordata&0xF0;	//д����λ
	LCDEN=1;lcd_delay(10);				//ʹ���á�1��
	LCDEN=0;lcd_delay(10);				//ʹ���塰0��
}

/**
 * @brief  Dis_Line str-Ҫ��ʾ���ַ��� ������16�� y��ʾ���� 1-��һ�� 2-�ڶ���
 * @param  ��
 * @retval ��
 */
static void Dis_Line(unsigned char *str, int y)
{
  unsigned char i=0;									//�������ѭ������i
  if(y==1)
  	LCD_fops.write(0x80,0);//ѡ���һ��
  else if(y==2)LCD_fops.write(0xC0,0);  //�ڶ���
 	lcd_delay(100);							//�ȴ�Һ����ʾ�ȶ�
  for(i=0;i<16;i++)			//��һ����д��16���ַ�
  {

		lcd_delay(10);				//�ȴ�Һ����ʾ�ȶ�
		LCD_fops.write(str[i],1); 		//д��˵�������
		lcd_delay(10);				//�ȴ�Һ����ʾ�ȶ� 

  }
}

/**
 * @brief  Dis_Char y��ʾ���� 1-��һ�� 2-�ڶ��� c-Ҫ��ʾ���ַ�  
 * @param  ��
 * @retval ��
 */
static void Dis_Char(int x, int y, char c)
{
  if(y==1)
  	LCD_fops.write(0x80+x,0);//ѡ���һ��
  else if(y==2)LCD_fops.write(0xC0+x,0);  //�ڶ���
 	lcd_delay(100);							//�ȴ�Һ����ʾ�ȶ�
   	LCD_fops.write(c,1);
}

/**
 * @brief  Dis_Str x��λ�� y��ʾ���� 1-��һ�� 2-�ڶ��� str-Ҫ��ʾ���ַ���  
 * @param  ��
 * @retval ��
 */
static void Dis_Str(int x, int y, char *str)
{
  if(y==1)
  	LCD_fops.write(0x80+x,0);//ѡ���һ��
  else if(y==2)LCD_fops.write(0xC0+x,0);  //�ڶ���
 	lcd_delay(100);							//�ȴ�Һ����ʾ�ȶ�
   	while(*str != '\0')
	{
		LCD_fops.write(*str,1);
		str++;
	}
}


/**
 * @brief  LCD_Init LCD��ʼ��
 * @param  ��
 * @retval ��
 */
void LCD_Init(void)
{
	//����P0.0-2Ϊ׼˫��/������ģʽ
	P0M0&=0xF8;			//P0M0.4-6=0
	P0M1&=0xF8;			//P0M1.4-6=0
	//����P2.4-7Ϊ׼˫��/������ģʽ
	P2M0&=0x0F;			//P2M0.4-7=0
	P2M1&=0x0F;			//P2M1.4-7=0
	lcd_delay(5);				//�ȴ�I/Oģʽ�����ȶ�
	LCDRW=0;				//��ֻ�漰д��������ʽ�RW����ֱ���õ�
	lcd_delay(5);				//�ȴ�I/Oģʽ�����ȶ�
	LCD_fops.write(0x32,0);lcd_delay(10);//0x32������μ�HD44780�����ֲ�
	LCD_fops.write(0x28,0);lcd_delay(10);//��������Ϊ4λ����ʾ2�У�5*7����/ÿ�ַ�
	LCD_fops.write(0x0C,0);lcd_delay(10);//���ÿ���ʾ������ʾ���
	LCD_fops.write(0x06,0);lcd_delay(10);//д�������ݺ���ʾ�����岻�ƶ����������
	LCD_fops.write(0x01,0);lcd_delay(10);//д����������

}
/**
 * @brief  LCD_Init LCD��ʱ
 * @param  ��
 * @retval ��
 */
static void lcd_delay(unsigned int Cnt)
{
  unsigned char i,j;
  while (Cnt--)//Count�βο�����ʱ����
  {
    for(i=0;i<50;i++)
      for(j=0;j<20;j++);
  }
}