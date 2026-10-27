#include<ioCC2530.h> 

#define uint unsigned int 

#define uchar unsigned char

#define LED1 P1_0  // P1.0 口控制 LED1 

#define LED2 P1_1
#define LED3 P1_4
#define S1 P0_0
uchar mode=0;
int light_value=0;                    //声明变量表示呼吸灯的亮度 

void InitLed(void);                // 初始化 LED 灯相应的  IO 口 
void InitTimer1(void);            //初始化定时器1   
void InitKey(void);
void Delayms(uint time);
void BlinkAll(void);
void RunningLight(void);
void BreathingLight(void);

void InitLed(void) 
 

{   

   P1SEL &= ~0x03;      //将P1.0定义为外设 
  
P1DIR |= 0x13;      // P1.0定义为输出   
  
LED1 = 0;              // LED1 灯初始化为熄灭状态 
   LED2 = 0;
   LED3 = 0;
  
} 

void InitKey(void)
{
   P0SEL &= ~0x01;
   P0DIR &= ~0x01;
   P0INP &= ~0x01;
   P2INP &= ~0x20;
}
 void InitTimer1 (void) 

 { 
   
   CLKCONCMD &= ~0x7F;              //设置系统时钟源为32MHZ晶振      0111   1111   
  
   while(CLKCONSTA & 0x40);        //等待晶振为32M稳定   
  
   CLKCONCMD &= ~0x47;            //设置系统主时钟频率为32MHZ   0100     0111 
   
 
 
 

   PERCFG |= 0x40;                          //timer1 IO 备用2 0100 0000           
  
   P2SEL &= ~0x10;                    //设置优先级，定时器1优先 
  
   P2DIR |= 0x80;                      //设定第1优先级：定时器1通道0-1 
 
  
   T1CC0L=0xff; 
 
   T1CC0H=0x00; 
 
   T1CTL |= 0x02;              // 0000 0010   模模式（从 0x0000到T1CC0反复计数） 

   T1CCTL2 = 0x1c;        //模式选择 0001 1100    01(使能比较中的事件) 110（通道控制比较模式6） 

   T1STAT &=~0x04;//通道2比较标志位(也可以不写) 
   
} 

void Delayms(uint time) //i=time 即延时  i 毫秒 
   
{ 

     uint i,j; 

     for(i=0;i<time;i++) 

       for(j=0;j<500;j++); 

   } 

uchar KeyPress(void)
{
   static uchar key_state = 0;
   if(S1 == 0)
   {
      Delayms(10);
      if(S1 == 0)
    {
        if(key_state == 0)
      {
        key_state = 1;
        return 1;
     }
    }
   }
   else
   {
      key_state = 0;
   }
   return 0;
}

void BlinkAll(void)
{
  LED1=1;
  LED2=1;
  LED3=1;
  Delayms(500);
  LED1=0;
  LED2=0;
  LED3=0;
  Delayms(300);
}

void RunningLight(void)
{
   LED1 = 1;
   LED2 = 0;
   LED3 = 0;
   Delayms(300);
   LED1 = 0;
   LED2 = 1;
   LED3 = 0;
   Delayms(300);
   LED1 = 0;
   LED2 = 0;
   LED3 = 1;
   Delayms(300);
}

void BreathingLight(void)
{
  static int dir = 1;
  if(dir == 1)
{
  light_value++;
  if(light_value>=255)
  {
    light_value=255;
    dir = -1;
  }
}
else
{
  light_value--;
  if(light_value <=0)
  {
    light_value=0;
    dir=1;
   }
  }
  T1CC2L = light_value;
  T1CC2H = 0x00;
  Delayms(5);
}
 void main(void) 

 { 
  
   InitLed();
   InitKey();
  
   InitTimer1(); 
 
   while(1) 
  
   { 
    
     if(KeyPress())
     {
     mode = (mode+1)%4;
     if(mode==3)
     {
     P1SEL |= 0X01;
     light_value = 0;
     }
     else
     {
     P1SEL &= ~0X01;
     LED1 = 0;
     LED2 = 0;
     LED3 = 0;
     }
     }
      switch(mode)
      {
      case 0:
      LED1 = 0;
      LED2 = 0;
      LED3 = 0;
      break;
      case 1:
      BlinkAll();
      break;
      case 2:
      RunningLight();
      break;
      case 3:
      BreathingLight();
      break;
      }
  
   }     

 }