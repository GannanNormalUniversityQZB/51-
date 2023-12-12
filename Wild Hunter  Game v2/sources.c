#include <reg52.h>
#include <intrins.h>
//����Һ�������λ
#define	RS_CLR RS=0
#define RS_SET RS=1
#define RW_CLR RW=0
#define RW_SET RW=1
#define EN_CLR EN=0
#define EN_SET EN=1
#define DataPort P0

sbit RS = P1 ^ 0;
sbit RW = P1 ^ 1;
sbit EN = P2 ^ 5;
sbit DU = P2 ^ 0;
sbit WE = P2 ^ 1;
//����ȫ�ֱ���
unsigned char KeyCount = 0;
sbit K3 = P3 ^ 6;
sbit K4 = P3 ^ 7;
sbit BEEP = P2 ^ 3;

unsigned char code cdis1[] = {"STOPWATCH      0"};
unsigned char code cdis2[] = {"---GNNU   QZB---"};
unsigned char code cdis3[] = {"TIME            "};
unsigned char code cdis4[] = {"BEGIN COUNT    1"};
unsigned char code cdis5[] = {"PAUSE COUNT    2"};
unsigned char code cdis6[] = {"BEGIN COUNT    3"};
unsigned char code cdis7[] = {"PAUSE COUNT    4"};
unsigned char code cdis8[] = {"    				  	 "};

unsigned char display[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char display2[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void Delay(unsigned int num)
{
	while(num--);
}

void beep()
{
	unsigned char y;
	for(y=0;y<180;y++)
	{
		BEEP = !BEEP;
		Delay(70);
	}
	BEEP = 1; 	// �رշ�����
}

void delay1(unsigned int ms)
{
	unsigned char n;
	while(ms--)
	{
		for(n=0;n<250;n++)
		{
			_nop_();
			_nop_();
			_nop_();
			_nop_();
		}
	}	
}

//��æ����
bit lcd_busy()
{
	bit result;
	RS_CLR;		//����
	RW_SET;		//��
	EN_SET;	//ʹ��
	_nop_();	//�յ�һ����������
	_nop_();	//�յ�һ����������
	_nop_();	//�յ�һ����������
	_nop_();	//�յ�һ����������
	result = (bit)(DataPort & 0x80);
	EN_CLR;	//ʹ����
	return result; //1000 0000
} 





//д�����
void lcd_wcmd(unsigned char com)
{
	while(lcd_busy()){}
	RS_CLR;	//����
	RW_CLR;	//д
	EN_CLR;
	_nop_();
	_nop_();
	DataPort = com;	//д������
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	EN_SET;	//ʹ��
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	EN_CLR;
	Delay(10);
}

//д�����ݺ���
void lcd_wdat(unsigned char Data)
{
	while(lcd_busy()){}
		RS_SET;	//����
		RW_CLR;	//д
		EN_CLR;
		DataPort = Data;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		EN_SET;	//ʹ��
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		EN_CLR;
		Delay(10);
}




void lcd_pos(unsigned char pos)
{
	
	lcd_wcmd(pos | 0x80); 
}


void lcd_init()
{
	RW_CLR;
	delay1(15);
	lcd_wcmd(0x01);	//���lcd����ʾ����
	lcd_wcmd(0x38);
	//��ʾģʽ
	delay1(5);
	lcd_wcmd(0x38);
	delay1(5);
	lcd_wcmd(0x38);
	delay1(5);
	lcd_wcmd(0x0C);//��ʾ�����������
	delay1(5);
	lcd_wcmd(0x01);	//���lcd����ʾ����
	delay1(5);
	
}

void play()
{
	unsigned char i;
	for(i=0;i<6;i++)
	{
		display2[i] = display[i] + 0x30;
	}
	//ʱ��λ���ݴ���
	display2[7] = display[6]/10 + 0x30;		//ʱ��λ���ݴ���
	display2[6] = display[6]%10 + 0x30;
	lcd_pos(0x45);
	//��ʾʱ
	lcd_wdat(display2[7]);
	lcd_wdat(display2[6]);
	lcd_wdat(0x3a);	// ��ʾ������
	//��ʾ��
	lcd_wdat(display2[5]);
	lcd_wdat(display2[4]);
	lcd_wdat(0x3a);	// ��ʾ������
	//��ʾ��
	lcd_wdat(display2[3]);
	lcd_wdat(display2[2]);
	lcd_wdat(0x3a);	// ��ʾ������
	//��ʾ��
	lcd_wdat(display2[1]);
	lcd_wdat(display2[0]);
	
}



void main()
{
	unsigned char m;
	TMOD = 0x01;	//������ʽһ16λ
	TH0 = 0xdc;		//50���붨ʱ��ԭ����0xdc
	TL0=0x00;	
	EA = 1;
	ET0=1;
	lcd_init();
	lcd_pos(0x00);	//������ʾλ��Ϊ��һ��
	for(m=0;m<16;m++)
		lcd_wdat(cdis1[m]);
	//��ʾ�ַ�
	lcd_pos(0x40);
	for(m=0;m<16;m++)
	{	
		//��ʾ�ַ�
		lcd_wdat(cdis2[m]);
		delay1(60);
	}
	delay1(1000);
	lcd_pos(0x40);//������ʾλ��Ϊ�ڶ���
	for(m=0;m<16;m++)
	{	
		//��ʾ�ַ�
		lcd_wdat(cdis3[m]);
	}
	while(1)
	{
		if(K3==0)
		{
			KeyCount++;	//����
			beep();
			delay1(100);
			switch(KeyCount)
			{
				case 1: TR0 = 1;
								lcd_pos(0x00);
								for(m=0;m<16;m++)
									lcd_wdat(cdis4[m]);
								break;
				case 2:TR0 = 0;
								lcd_pos(0x00);
							for(m=0;m<16;m++)
									lcd_wdat(cdis5[m]);
							break;
				case 3:TR0 = 1;
								lcd_pos(0x00);
							for(m=0;m<16;m++)
									lcd_wdat(cdis6[m]);
							break;
				case 4:TR0 = 0;
								lcd_pos(0x00);
							for(m=0;m<16;m++)
									lcd_wdat(cdis7[m]);
							break;
				default:	TR0 = 0;
									break;
			}
		}
		if(K4 == 0)
		{
			TR0 = 0;
			KeyCount = 0;
			for(m=0;m<8;m++)
					display[m] = 0x00;	//��ʱ��Ԫ����
			lcd_pos(0x00);		//������ʾλ�õ�һ��
			for(m=0;m<16;m++)
					lcd_wdat(cdis1[m]);
			beep();
			delay1(100);
		}
		play();
	}
	
}

//�жϺ���
void Time0() interrupt 1 using 0
{
		TH0=0xdc;		//	10ms��ʱ
		TL0=0x00;
		display[0]++;	//0.01s
		if(display[0] == 10)
		{
			display[0] = 0;
			display[1]++;	//0.1s 
		}
		if(display[1] == 10)
		{
			display[1] = 0;
			display[2]++;	//1s 
		}
		if(display[2] == 10)
		{
			display[2] = 0;
			display[3]++;	//��ʮλ
		}
		if(display[3] == 6)
		{
			display[3] = 0;
			display[4]++;	// �ָ�λ
		}
		if(display[4] == 10)
		{
			display[4] = 0;
			display[5]++;	//��ʮλ 
		}
		if(display[5] == 6)
		{
			display[5] = 0;
			display[6]++;	//ʱ
		}
		if(display[6] == 24)
		{
			display[6] = 0;
		}
}
		











