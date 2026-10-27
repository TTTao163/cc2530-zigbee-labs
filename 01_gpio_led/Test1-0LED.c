#include<iocc2530.h>
#define LED1 P1_0
#define LED2 P1_1
#define LED3 P1_2

void IntLed(void)
{
  P1SEL &=~0x13;
  P1DIR |=0x13;

  LED1 = 0;
  LED2 = 0;
  LED3 = 0;
}

void main(void)
{ 
 IntLed();
 while(1)
 {
  LED1 = 1; 
  LED2 = 1; 
  LED3 = 1; 
 }
}