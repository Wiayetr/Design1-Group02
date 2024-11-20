#include "msp430g2553.h" // 包含msp430g2553单片机的头文件
#include "stdlib.h"     // 包含标准库头文件

#define GATE_0_2v  56       // 定义一个宏，用于将0.2V电压转换为ADC的数值，以3.6V为参考电压

unsigned int Num=0;       // 定义一个无符号整型变量Num，用于计数
unsigned int First_votage,votage_temp; // 定义两个无符号整型变量，用于存储电压值

void main(void)
{
  unsigned int i=0,j=0;    // 定义两个无符号整型变量i和j，用于循环计数
  int *Flash_ptr = (int *) 0xd000; // 将Flash的地址0xd000转换为整型指针
  unsigned int flash_time; // 定义一个无符号整型变量flash_time，用于计时

  WDTCTL = WDTPW + WDTHOLD; // 停止看门狗计时器
  ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // 配置ADC10，开启ADC，允许中断
  ADC10CTL1 = INCH_1;       // 设置ADC输入通道为A1
  ADC10AE0 |= 0x02;         // 启用PA.1作为ADC通道
  P1DIR |= 0x01;            // 设置P1.0为输出方向
  P1DIR &= ~BIT3;           // 设置P1.3为输入方向
  P1OUT |= BIT3;            // 将P1.3置高电平
  P1REN |= BIT3;            // 启用P1.3的电阻
  First_votage=0;           // 初始化First_votage为0
  for(i=0;i<32;i++)         // 采集32次ADC值并累加到First_votage中
  {
      ADC10CTL0 |= ENC + ADC10SC; // 开始ADC采样和转换
      __bis_SR_register(CPUOFF + GIE); // 进入低功耗模式0，等待ADC中断
      First_votage += ADC10MEM;      // 累加ADC结果
  }
  First_votage = First_votage >> 5; // 计算32次采样的平均值

  P1OUT &= ~BIT0;                   // 将P1.0置低电平，LED熄灭
  P1OUT |= BIT0;                    // 将P1.0置高电平，LED点亮
  flash_time=0;                     // 初始化flash_time为0
  do
  {
     if(!(P1IN & BIT3))             // 如果P1.3为低电平（按键按下），则跳出循环
         break;
     flash_time++;                  // 计时器增加
     if(flash_time == 300)          // 如果计时器达到300，则点亮LED
         P1OUT |= BIT0;
     if(flash_time == 600)         // 如果计时器达到600，则熄灭LED并重置计时器
     {    P1OUT &= ~BIT0;  flash_time=0;   }

     votage_temp=0;                 // 初始化votage_temp为0
     for(i=0;i<32;i++)             // 再次采集32次ADC值并累加到votage_temp中
     {
        ADC10CTL0 |= ENC + ADC10SC; // 开始ADC采样和转换
        __bis_SR_register(CPUOFF + GIE); // 进入低功耗模式0，等待ADC中断
        votage_temp += ADC10MEM;    // 累加ADC结果
     }
     votage_temp = votage_temp >> 5; // 计算32次采样的平均值
     votage_temp = abs(votage_temp-First_votage); // 计算两次采样的电压差
  }while(votage_temp < GATE_0_2v);  // 如果电压变化超过0.2V，进入下面的记录代码！

  flash_init();                     // 初始化Flash
  erase_en_write_D000();             // 擦除并写入地址0xd000处的数据，最大5120个采样点

  P1OUT |= BIT0;                    // 点亮LED
  for (;;)                          // 循环直到存储满4096字节（2048个单词）！
  {
    ADC10CTL0 |= ENC + ADC10SC;     // 开始ADC采样和转换
    __bis_SR_register(CPUOFF + GIE); // 进入低功耗模式0，等待ADC中断

    *Flash_ptr++ = ADC10MEM;        // 将ADC数据保存到Flash（0xd000）
    j++;
    if(j >=(2048+2048))             // 检查是否已经存储了足够的数据
        break;
  }
  close_write_flash();               // 关闭Flash写入
  P1OUT = 0;                         // 关闭所有P1端口的输出

  while(1);                          // 进入死循环
}

// ADC10中断服务程序
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  {__bic_SR_register_on_exit(CPUOFF);}  // 清除CPUOFF位，从低功耗模式退出 }
  Num++;                             // Num计数器增加
}
