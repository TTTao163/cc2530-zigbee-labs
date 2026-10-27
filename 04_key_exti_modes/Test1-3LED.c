#include <ioCC2530.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define LED1 P1_0    // P1.0 口控制 LED1
#define LED2 P1_1    // P1.1 口控制 LED2
#define LED3 P1_4    // P1.4 口控制 LED3
#define S1 P0_0      // P0.0 口控制 S1

void InitLED(void);
void Delayms(uint time);
void InitKEY(void);
__interrupt void P0_ISR(void);

// 全局变量
uchar mode = 0;  // 当前功能模式

void InitLED(void)
{
    P1SEL &= ~0x13;     // 设置P1.0、P1.1、P1.4为普通IO口
    P1DIR |= 0x13;      // P1.0、P1.1、P1.4定义为输出
    LED1 = 0;           // LED1灯初始化为熄灭状态
    LED2 = 0;           // LED2灯初始化为熄灭状态
    LED3 = 0;           // LED3灯初始化为熄灭状态
}

void Delayms(uint time)
{
    uint i, j;
    for (i = 0; i < time; i++)
        for (j = 0; j < 240; j++);
}

void InitKEY(void)
{
  P0IEN |= 0x01; // 0000 0001-XXXX XXX1
  IEN1 |= 0x20; // 0010 0000-XX1X XXXX
  PICTL |= 0x01; // 0000 0001-XXXX XXX1
  P0IFG &= ~0x01; //0000 0001-XXXX XXX0
  //IRCON &= ~0x20; //0010 0000-XX0X XXXX
  EA = 1;
}

#pragma vector = P0INT_VECTOR
__interrupt void P0_ISR(void)
{
    Delayms(10); // 消抖
    if (S1 == 0)
    {
        mode = (mode + 1) % 5; // 循环模式
        while (!S1);           // 等待松手
    }
    P0IFG &=~0x01;//清中断标志
    P0IF = 0;//清中断标志

}

void main(void)
{
    InitLED();
    InitKEY();

    while (1)
    {
        switch (mode)
        {
            case 1: // 点亮LED1
                LED1 = 1;
                break;

            case 2: // LED1、LED2、LED3同时闪烁2次
                LED1 = LED2 = LED3 = 1; Delayms(1000);
                LED1 = LED2 = LED3 = 0; Delayms(1000);
                LED1 = LED2 = LED3 = 1; Delayms(1000);
                LED1 = LED2 = LED3 = 0; Delayms(1000);
                break;

            case 3: // 跑马灯
                LED1 = 1; LED2 = 0; LED3 = 0; Delayms(500);
                LED1 = 0; LED2 = 1; LED3 = 0; Delayms(500);
                LED1 = 0; LED2 = 0; LED3 = 1; Delayms(500);
                break;
                
            case 4:
                LED1 = 0; LED2 = 0; LED3 = 0;
                break;

            default:
                break;
        }
    }
}