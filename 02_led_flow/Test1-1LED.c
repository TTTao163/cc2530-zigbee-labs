/***************
Function：操作IO口控制网蜂功能底板上红色LED、绿色LED每隔0.5s同时闪烁一次。
Author：ZT
Data：2025.3.16
Version：V1-0
**************/

/*头文件区域*/
#include<iocc2530.h>

/*定义LED灯的端口*/
#define LED1 P1_0
#define LED2 P1_1
#define LED3 P1_4

/*定义变量*/
#define uint unsigned int
#define uchar unsigned char

/*函数声明*/
void Delayms(uint time);
void IOInit(void);

void Delayms(uint time)
{
  uint i;
  uchar j;
  for(i=0;i<time;i++)
  {
    for(j=0;j<240;j++)
    {
      asm("NOP");
      asm("NOP");
      asm("NOP");
    }
  }
}

void IOInit(void)
{
  P1SEL &= ~0x13;
  P1DIR |= 0x13;
  LED1 = 0;
}

/*Main函数*/
void main()
{
 IOInit();
  while(1)
  {
    LED1 = 1;
    Delayms(500);
    LED1 = 0;
    Delayms(500);
    LED2 = 1;
    Delayms(500);
    LED2 = 0;
    Delayms(500);
    LED3 = 1;
    Delayms(500);
    LED3 = 0;
    Delayms(500);
  }
}