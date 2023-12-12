/***********************************************************************
*@program:Wilderness Hunter Game
*@author:Gannan Normal University ZhiBin Qiu
*@date:2023-12-01
*@version:1.0
************************************************************************/
#include <reg52.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//define area
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
sbit k2 = P3 ^ 4;
sbit k3 = P3 ^ 5;
sbit k4 = P3 ^ 6;
sbit k5 = P3 ^ 7;

unsigned char code cdis1[] = {"--Wild  Hunter--"};
unsigned char code cdis2[] = {"-let's gaming !-"};
unsigned char code cdis3[] = {"start:k4 back:k5"};
unsigned char code cdis4[] = {"-thanks for playing--thanks for playing-"};
unsigned char code cdis5[] = {"(=^_^=) (=^_^=) (=^_^=) (=^_^=) (=^_^=) "};
unsigned char code cdis6[] = {"S:              "};
unsigned char code cdis7[] = {"L:              "};
unsigned char code cdis8[] = {"                "};
unsigned char code cdis9[] = {"EASY:K4  HARD:K5"};
unsigned char  place[2] = {0,0};
unsigned char map[2][8] ={{' ',' ',' ',' ',' ',' ',' ',' '},{' ',' ',' ',' ',' ',' ',' ',' '}};


//辅助变量
unsigned char countK2 = 0;
unsigned char countK3 = 0;
unsigned char countK4 = 0;
unsigned char countK5 = 0;
unsigned char count;
unsigned char T;
unsigned char isfind=0;
unsigned char row;
unsigned char col;
unsigned char score=0;
char difficulty = 0;
//declare functions
void cmg88()
{	
	//关闭数码管，点阵函数
	DU = 1;
	P0=0x00;
	DU =0;
}

void DelayUs2x(unsigned char t)
{
	while(t--);
}

void DelayMs(unsigned int t)
{
	while(t--)
	{
		//大致延时1ms
		DelayUs2x(245);
		DelayUs2x(245);
	}
}

//判忙函数
bit LCD_Check_Busy()
{
	DataPort = 0xff;
	RS_CLR;		//命令
	RW_SET;		//读
	EN_CLR;	//使不能
	_nop_();	//空等一个机器周期
	EN_SET;	//使能
	return (bit)(DataPort & 0x80); //1000 0000
}




//写命令函数
void LCD_Write_Com(unsigned char com)
{
	while(LCD_Check_Busy()){}
	RS_CLR;	//命令
	RW_CLR;	//写
	EN_SET;	//使能
	DataPort = com;	//写入数据
	_nop_();	
	EN_CLR;
}

//写入数据函数
void LCD_Write_Data(unsigned char Data)
{
	while(LCD_Check_Busy()){}
		RS_SET;	//数据
		RW_CLR;	//写
		EN_SET;	//使能
		DataPort = Data;
		_nop_();
		EN_CLR;
}

//清屏函数
void LCD_Clear()
{
	LCD_Write_Com(0x01);
	DelayMs(5);
}

void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s)
{
	if(y==0)
	{
		LCD_Write_Com(0x80 + x);//表示第一行
	}
	else
	{
		LCD_Write_Com(0xc0 + x);//表示第二行
	}
	while(*s)
	{
		LCD_Write_Data(*s);
		s++;
		//DelayMs(500);
	}
}

void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data)
{
	if(y==0)
	{
		LCD_Write_Com(0x80+x);
	}
	else
	{
		LCD_Write_Com(0xc0+x);
	}
	LCD_Write_Data(Data);
}

void LCD_init()
{
	LCD_Write_Com(0x38);
	//显示模式
	DelayMs(5);
	LCD_Write_Com(0x38);
	DelayMs(5);
	LCD_Write_Com(0x38);
	DelayMs(5);
	LCD_Write_Com(0x38);
	LCD_Write_Com(0x08);//关闭显示
	LCD_Write_Com(0x01);//显示清屏
	LCD_Write_Com(0x06);//显示光标移动位置
	DelayMs(5);
	LCD_Write_Com(0x0c);//显示开及光标设置
	
}

void gameLevels()
{
	
	do
	{
		place[0] = rand()%2;		//行
	}while(place[0]==row);
	
	do
	{
		place[1] = 8 + rand()%8;   //列
	}while(place[1]==col);
	
}


void keyScan()
{
	char i=row,j=col;
	char isPress=0;
	if(k2==0)
	{	
		row=0;
		isPress=1;
	}
	else if(k3==0)
	{	
		row=1;
		isPress=1;
	}
	else if(k4==0)
	{
		while(k4==0);
		col--;
		if(col<8)
			col=8;
		isPress=1;
	}
	else if(k5==0)
	{
		while(k5==0);
		col++;
		if(col>15)
			col=15;
		isPress=1;
	}
	//写入数据
	if(isPress==1)
	{
	 LCD_Write_Char(j,i,' ');
	 LCD_Write_Char(col,row,'H');
	}
	if(row == place[0] && col == place[1])
	{
		TR0 = 0; //关闭定时器
		isfind = 1;
		LCD_Write_String(8,0,"You win!");
		LCD_Write_String(8,1,"  next  ");
		score++;
		LCD_Write_Char(2,1,score/10+48);
		LCD_Write_Char(3,1,score%10+48);
		DelayMs(500);
	}
	
}

void clearKs()
{
	//上下左右键重置
	row=0;
	col=8;
}

void genMole()
{	
	int i,j;
	gameLevels();
	//映射到lcd1602
	for(i=0;i<2;i++)
		for(j=8;j<=15;j++)
		{
			if(i==place[0] && j==place[1])
					LCD_Write_Char(j,i,'P');
			else
				LCD_Write_Char(j,i,' ');
 		}
	//保持狩猎者位置
	LCD_Write_Char(col,row,'H');
}

void gamePlay()
{
	int i; 
	LCD_Write_String(0,1,cdis9);
	while(1)
	{
		if(k4 == 0)
		{
			while(k4 == 0);
			difficulty = 0;
			break;
		}
		else if(k5 == 0)
		{
			while(k5 == 0);
			difficulty = 1;
			break;
		}
	}
	LCD_Write_String(0,0,"Are you ready ? ");
	LCD_Write_String(0,1,cdis8);
	for(i='3';i>='1';i--)
	{
		LCD_Write_Char(7,1,i);
		DelayMs(500);
	}
	LCD_Clear();		//清屏
	LCD_Write_String(0,0,cdis7);
	LCD_Write_String(0,1,cdis6);
	LCD_Write_Char(2,1,'0');
	LCD_Write_Char(3,1,'0');
	//设置倒计时
	LCD_Write_Char(6,0,'T');
	//重置按键
		clearKs();
	//游戏开始
	for(i=1;;i++)
	{	
		//显示level
		LCD_Write_Char(2,0,i/10+48);
		LCD_Write_Char(3,0,i%10+48);
		//生成随机数，模拟地鼠出现位置
		srand(rand());
		genMole();
		
		//定时器设置
		count =0;
		T = 5;
		TMOD = 0X01;
		TH0 = 0X4C;
		TL0 = 0X00;	//计时50ms
		EA = 1; // 全局中断开 
		ET0 = 1; //定时器0中断开
		TR0 = 1;	//开始计时
		//重置isfind
		isfind =0;
		LCD_Write_Char(6,1,T+48);
		while(T>0)
		{	
			if(isfind == 1)
			{
				break;  
			}
			keyScan();
		}
		if(T<=0)
		{
			TR0=0;	//关闭定时器
			LCD_Write_String(8,0,"You lose");
			LCD_Write_String(8,1,"  over  ");	
			// 重置分数
			score = 0;
			DelayMs(500);
			break;
		}
			
	}
}

void main()
{	
	int i;
	cmg88();
	//关闭数码管，点阵函数
	LCD_init();
	LCD_Clear();		//清屏
	LCD_Write_String(0,0,"--Wild  Hunter--");
	for(i=0;i<16;i++)
	{
		LCD_Write_Char(i,1,cdis2[i]);
		DelayMs(60);
	}
	DelayMs(200);
	LCD_Write_String(0,1,cdis3);
	while(1)
	{
		if(k4 == 0)
		{
			while(k4 == 0);		//软件防抖
				gamePlay();
			DelayMs(1000);
			LCD_Clear();		//清屏
			LCD_Write_String(0,0,"--Wild  Hunter--");
      for(i=0;i<16;i++)
      {
      	LCD_Write_Char(i,1,cdis2[i]);
      	DelayMs(60);
      }
      DelayMs(200);
      LCD_Write_String(0,1,cdis3);
		}
		else if(k5 == 0)
		{	
			while(k5 == 0);		//软件防抖
			LCD_Clear();		//清屏
			DelayMs(300);
			LCD_Write_String(0,0,cdis5);
			LCD_Write_String(0,1,cdis4);
			DelayMs(300);
			while(1)
			{
				LCD_Write_Com(0x18);//滚动显示
				DelayMs(350);
			}
		}
	}
	while(1);
	
	
}
	
void time0() interrupt 1
{
	 if(count<=20)
		 count++;
	 else
	 {
		 T--;
		 if((T == 3 || T == 1) && difficulty == 1)
			 genMole();
		 LCD_Write_Char(6,1,T+48);
		count = 0;
		TH0 = 0X4C;
		TL0 = 0X00;	//计时50ms
		EA = 1; // 全局中断开 
		ET0 = 1; //定时器0中断开
		TR0 = 1;	//开始计时
	 }
}
	
	
	
	
	











