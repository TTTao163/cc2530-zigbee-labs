# CC2530 单片机与 ZigBee 无线传感网实验

基于 **TI CC2530F256**（8051 内核，2.4GHz IEEE 802.15.4 射频）的裸机外设实验合集，全部使用**寄存器级编程**（`ioCC2530.h`），不依赖 Z-Stack 协议栈，用于打牢 8051 内核 + 无线 SoC 的底层基础。

12 个实验覆盖 GPIO、外部中断、定时器（查询/中断/PWM 三种用法）、低功耗休眠唤醒、UART、ADC 与传感器驱动。

## 硬件平台

| 项目 | 参数 |
|---|---|
| MCU | TI CC2530F256（增强型 8051，256KB Flash，8KB RAM） |
| 系统时钟 | 32MHz 晶振（部分实验默认 16MHz 内部 RC） |
| 射频 | 2.4GHz IEEE 802.15.4，可配 ZigBee / RF4CE |
| 开发环境 | IAR Embedded Workbench for 8051（8.10 及以上） |
| 板载资源 | LED1~LED3、按键 S1、串口 UART0（P0.2/P0.3）、DHT11 温湿度 |

## 实验清单

| 目录 | 实验内容 | 关键寄存器 / 知识点 |
|---|---|---|
| `01_gpio_led` | GPIO 输出，点亮三路 LED | `P1SEL` 功能选择、`P1DIR` 方向配置、`P1_0~P1_4` 位操作 |
| `02_led_flow` | 跑马灯，三灯轮流点亮 | 软件延时 `Delayms()` 的循环次数与主频换算 |
| `03_key_control_led` | 按键控制跑马灯与同步闪烁切换 | 按键扫描 `KeyScanS1()`、状态变量驱动的多模式切换 |
| `04_key_exti_modes` | 外部中断按键控制 5 种 LED 模式 | `P0IEN` 中断使能、`PICTL` 边沿配置、`P0IFG` 标志清零、`#pragma vector = P0INT_VECTOR` 中断服务函数、中断内软件消抖 |
| `05_timer_polling` | 定时器 T1 查询方式定时翻转 LED | `T1CTL` 分频与模式、`IRCON` 中断标志轮询（不使能中断） |
| `06_timer_t3_interrupt` | 定时器 T3 中断方式定时 | T3 溢出中断、中断服务函数中翻转 LED、主循环空转的低功耗写法 |
| `07_pwm_timer1` | 定时器 T1 比较输出 PWM 调光 | `T1CC0` 模模式周期、`T1CCTL2` 通道比较模式 6、占空比与亮度关系 |
| `08_pm_sleep_wakeup` | 低功耗休眠 + 定时器唤醒 | `PCON` 电源模式位（PM1/PM2）、睡眠定时器唤醒、唤醒后 LED 闪 3 次提示 |
| `09_uart_send` | 系统时钟切换 32MHz + UART0 定时发送 | `CLKCONCMD` / `CLKCONSTA` 时钟源切换与稳定等待、`U0CSR` 设 UART 模式、`U0BAUD=216` 对应 115200bps、`U0DBUF` 收发缓冲 |
| `10_uart_interrupt` | 串口中断收发与状态机 | `UART0_ISR` 中断处理、收/发状态机 `UartState`、不定长数据接收 |
| `11_adc_internal_temp` | ADC 采样片内温度传感器 | `ADCCON3` 参考电压与分辨率选择（1.25V / 14 位）、`ADCCON1` 手动启动、`ADCL`+`ADCH` 12 位结果拼接（注意低 4 位无效）、温度换算公式 |
| `12_dht11_uart` | DHT11 温湿度采集并串口上报 | 单总线时序（起始信号、40bit 数据、校验和）、`DHT11.c` 驱动封装、与 UART 驱动分层组合 |

## 目录结构

```
04_key_exti_modes/
├── Test1-3LED.c        # 主程序（寄存器级实现）
├── Test1-3LED.ewp      # IAR 工程文件
├── Test1-3LED.eww      # IAR 工作空间
└── Test1-3LED.ewd      # 调试器配置
```

## 编译与下载

1. 安装 **IAR Embedded Workbench for 8051**
2. 打开对应目录下的 `*.eww` 工作空间
3. `Project → Rebuild All` 编译
4. 用 CC Debugger + SmartRF04EB 下载到开发板

> 编译产物（`Debug/`、`settings/`、`*.r51`、`*.d51`、`*.hex`、`*.pbd`）已从仓库剔除。

## 关于编码

源码原本为 **GBK 编码**，仓库中的文件已统一转换为 **UTF-8**，以便在 GitHub 上正常显示中文注释。若用 IAR 打开出现中文乱码，可在 `Tools → Options → Editor → Encoding` 中设为 `UTF-8`。

## 参考资料

- [TI CC2530 产品页与数据手册](https://www.ti.com/product/CC2530)（原始中文数据手册体积较大，未纳入仓库）

## 说明

- 全部代码为寄存器操作，注释标明了每一位的二进制含义，便于对照手册理解
- 作者：张涛（物联2311）
