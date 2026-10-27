/****************************************************************************
* 文 件 名: main.c
* 作 者: ZT
* 修 订: 2025-04-25
* 版 本: 2.0
* 描 述: 设置串口调试助手波特率：115200bps 8N1
* 串口调试助手给 CC2530 字符串控制 Led 灯，命令如下：
* D1#/d1#开关 Led1 ; D2#/d2#开关 Led2 ; D3#/d3#开关 Led3;
* A0#点亮 Led1、Led2 和 Led3； A1#关闭 Led1、Led2 和 Led3；
* B0#开启呼吸灯； B1#关闭呼吸灯
****************************************************************************/
#include <ioCC2530.h>
#include <string.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define UART0_RX 1
#define UART0_TX 2
#define CONTROL_LED 3
#define SIZE 4 
#define ON 1
#define OFF 0

#define LED1 P1_0 //定义 P1.0 口为 LED1 控制端
#define LED2 P1_1 //定义 P1.1 口为 LED2 控制端
#define LED3 P1_4

char RxBuf;
char UartState;
uchar count;
char RxData[SIZE]; //存储发送字符串

// 呼吸灯相关变量
int light_value = 0;                    // 表示呼吸灯的亮度 
int dir = 1;                            // 亮度变化方向，1 为增加，-1 为减少
uchar breathing_mode = 0;               // 呼吸灯模式标志，1 表示开启，0 表示关闭

/****************************************************************************
* 名 称: DelayMS()
* 功 能: 以毫秒为单位延时
* 入口参数: msec 延时参数，值越大，延时越久
* 出口参数: 无
****************************************************************************/
void DelayMS(uint msec)
{
    uint i, j;
    for (i = 0; i < msec; i++)
        for (j = 0; j < 1070; j++);
}

/****************************************************************************
* 名 称: InitLed()
* 功 能: 设置 LED 灯相应的 IO 口
* 入口参数: 无 * 出口参数: 无
****************************************************************************/
void InitLed(void)
{
    P1DIR |= 0x13; // P1.0 和 P1.1 定义为输出口
    P1DIR |= 0x10; // P1.4 定义为输出口
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
}

/****************************************************************************
* 名 称: InitUart()
* 功 能: 串口初始化函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitUart(void)
{
    PERCFG = 0x00; //外设控制寄存器 USART 0 的 IO 位置:0 为 P0 口位置 1
    P0SEL = 0x0c; // P0_2,P0_3 用作串口（外设功能）
    P2DIR &= ~0xC0; // P0 优先作为 UART0
    U0CSR |= 0x80; // 设置为 UART 方式
    U0GCR |= 11;
    U0BAUD |= 216; // 波特率设为 115200
    UTX0IF = 0; // UART0 TX 中断标志初始置位 0
    U0CSR |= 0x40; // 允许接收
    IEN0 |= 0x84; // 开总中断允许接收中断
}

/****************************************************************************
* 名 称: UartSendString()
* 功 能: 串口发送函数
* 入口参数: Data:发送缓冲区 len:发送长度
* 出口参数: 无
****************************************************************************/
void UartSendString(char *Data, int len)
{
    // 定义变量i，用于循环
    uint i;
    // 循环遍历Data数组，每次发送一个字符
    for (i = 0; i < len; i++)
    {
        // 将Data数组中的字符发送到串口
        U0DBUF = *Data++;
        // 等待发送完成
        while (UTX0IF == 0);
        // 清除发送完成标志
        UTX0IF = 0;
    }
}

/****************************************************************************
* 名 称: UART0_ISR(void) 串口中断处理函数
* 描 述: 当串口 0 产生接收中断，将收到的数据保存在 RxBuf 中
****************************************************************************/
#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
    URX0IF = 0; // 清中断标志
    RxBuf = U0DBUF;
}

/****************************************************************************
* 名 称: InitTimer1()
* 功 能: 初始化定时器1
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitTimer1 (void) 
{ 
    CLKCONCMD &= ~0x7F;              //设置系统时钟源为32MHZ晶振      0111   1111   
    while(CLKCONSTA & 0x40);        //等待晶振为32M稳定   
    CLKCONCMD &= ~0x47;            //设置系统主时钟频率为32MHZ   0100     0111 
 
    PERCFG |= 0x40;                          //timer1 IO 备用2 0100 0000           
    P2SEL &= ~0x10;                    //设置优先级，定时器1优先 
    P2DIR |= 0x80;                      //设定第1优先级：定时器1通道0-1 
 
    T1CC0L = 0xff; 
    T1CC0H = 0x00; 
    T1CTL |= 0x02;              // 0000 0010   模模式（从 0x0000到T1CC0反复计数） 
    T1CCTL2 = 0x1c;        //模式选择 0001 1100    01(使能比较中的事件) 110（通道控制比较模式6） 
    T1STAT &=~0x04; //通道2比较标志位(也可以不写)
}

/****************************************************************************
* 名 称: BreathingLight()
* 功 能: 实现呼吸灯效果
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void BreathingLight(void)
{
    if (breathing_mode) {
        if (dir == 1)
        {
            light_value++;
            if (light_value >= 255)
            {
                light_value = 255;
                dir = -1;
            }
        }
        else
        {
            light_value--;
            if (light_value <= 0)
            {
                light_value = 0;
                dir = 1;
            }
        }
        T1CC2L = light_value;
        T1CC2H = 0x00;
        DelayMS(5);
    }
}

/****************************************************************************
* 程序入口函数
****************************************************************************/
void main(void)
{
    CLKCONCMD &= ~0x40; //设置系统时钟源为 32MHZ 晶振
    while(CLKCONSTA & 0x40); //等待晶振稳定为 32M
    CLKCONCMD &= ~0x47; //设置系统主时钟频率为 32MHZ

    InitLed(); //设置 LED 灯相应的 IO 口
    InitUart(); //串口初始化函数
    InitTimer1(); // 初始化定时器 1

    UartState = UART0_RX; //串口 0 默认处于接收模式
    memset(RxData, 0, SIZE); //清空接收缓冲区

    while (1)
    {
        if (UartState == UART0_RX) //接收状态
        {
            if (RxBuf != 0)
            {
                //以'＃'为结束符
                if ((RxBuf != '#') && (count < 3))
                    RxData[count++] = RxBuf;
                else
                {
                    if (count >= 3) //判断数据合法性，防止溢出
                    {
                        count = 0; //计数清 0
                        memset(RxData, 0, SIZE);//清空接收缓冲区
                    }
                    else
                        UartState = CONTROL_LED;//进入发送状态
                }
                RxBuf = 0;
            }
        }
        if (UartState == CONTROL_LED) //控制 LED 灯
        {
            //判断接收的数据合法性
            if ((RxData[0] == 'd' || RxData[0] == 'D') && (RxData[1] == '1' || RxData[1] == '2' || RxData[1] == '3'))
            {
                if (RxData[1] == '1')
                    LED1 = ~LED1; //高电平点亮
                else if (RxData[1] == '2')
                    LED2 = ~LED2;
                else if (RxData[1] == '3')
                    LED3 = ~LED3;
            }
            else if ((RxData[0] == 'a' || RxData[0] == 'A') && (RxData[1] == '0' || RxData[1] == '1'))
            {
                if (RxData[1] == '0')
                {
                    LED1 = ON;
                    LED2 = ON;
                    LED3 = ON;
                }
                else
                {
                    LED1 = OFF;
                    LED2 = OFF;
                    LED3 = OFF;
                }
            }
            else if ((RxData[0] == 'b' || RxData[0] == 'B') && (RxData[1] == '0' || RxData[1] == '1'))
            {
                if (RxData[1] == '0')
                {
                    breathing_mode = 1;
                    light_value = 0;
                    P1SEL |= 0X01; 
                }
                else
                {
                    breathing_mode = 0;
                    P1SEL &= ~0X01; 
                    LED1 = 0;
                    LED2 = 0;
                    LED3 = 0;
                }
            }
            UartState = UART0_RX;
            count = 0;
            memset(RxData, 0, SIZE); //清空接收缓冲区
        }
        BreathingLight(); // 持续执行呼吸灯函数，以实现呼吸灯效果
    }
}
