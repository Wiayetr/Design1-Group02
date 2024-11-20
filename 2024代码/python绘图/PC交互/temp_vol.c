#include <msp430.h>
#include "stdint.h"

#define VOLTAGE 0
#define TEMPERATURE 1

// 存储pc机发送的命令
volatile uint8_t command = 0;

// 电压测量的全局变量
// 定义一个数组，用来存储ADC转换的结果
uint16_t adcbuff[50] = {0};
// 定义变量，用于存储ADC转换结果的最大值和最小值
uint16_t maxval = 0;
uint16_t minval = 0;
// 定义变量，用于存储ADC转换结果的峰峰值（Vpp）
uint16_t vpp = 0;


// 温度测量的全局变量
long temp; // 存储原始温度传感器读值
float IntDegF; // 存储华氏度温度值
float IntDegC; // 存储摄氏度温度值


/*
* @fn:     void InitUART(void)
* @brief:  初始化串口，包括设置波特率，数据位，校验位等
* @para:   none
* @return: none
* @comment:初始化串口
*/
void InitUART(void) {
    UCA0CTL1 |= UCSWRST;
    UCA0CTL0 &= ~UCSYNC;
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 104;  // Baud rate 9600 at 1MHz
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    UCA0CTL1 &= ~UCSWRST;
    IE2 |= UCA0RXIE;  // Enable USCI_A0 RX interrupt
}

/*
* @fn:     void InitSystemClock(void)
* @brief:  初始化系统时钟
* @para:   none
* @return: none
* @comment:初始化系统时钟
*/
void InitSystemClock(void)
{
    /*配置DCO为1MHz*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    /*配置SMCLK的时钟源为DCO*/
    BCSCTL2 &= ~SELS;
    /*SMCLK的分频系数置为1*/
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

/*
* @fn:     void UARTSendByte(uint8_t byte)
* @brief:  通过UART发送一个字节
* @para:   byte:要发送的字节
* @return: 无
* @comment:等待上一个字节发送完成，然后发送新的字节
*/
void UARTSendByte(uint8_t byte) {
    // 等待上一个字符发送完毕
    while (UCA0STAT & UCBUSY);
    UCA0TXBUF = byte; // 发送当前字符
}


/*
* @fn:     void UARTSendString(uint8_t *pbuff)
* @brief:  通过UART发送字符串
* @para:   pbuff:要发送的字符串指针
* @return: 无
* @comment:发送字符串直到遇到字符串结束符'\0'
*/
void UARTSendString(uint8_t *pbuff) {
    while (*pbuff) {
        UARTSendByte(*pbuff++);
        while (UCA0STAT & UCBUSY);
    }
}


/*
* @fn:     void PrintNumber(int num)
* @brief:  通过串口发送数字
* @para:   num:要发送的数字
* @return: 无
* @comment:将整数转换为字符串并通过串口发送
*/
void PrintNumber(int num) {
    char buffer[12];
    int i = 0;

    // 处理负数
    if (num < 0) {
        buffer[i++] = '-';
        num = -num;
    }

    // 处理正数和零
    int len = 0;
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
        len++;
    }
    if (len == 0) {  // 处理零的情况
        buffer[i++] = '0';
    }

    // 反转字符串
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    buffer[i] = '\0'; // 添加字符串结束符

    UARTSendString((uint8_t *)buffer); // 发送转换后的字符串
}

/*
* @fn:     void PrintFloat(float num, int decimalPlaces)
* @brief:  通过串口发送浮点数
* @para:   num:要发送的浮点数
*         decimalPlaces:小数点后的位数
* @return: 无
* @comment:将浮点数转换为字符串并通过串口发送，包括整数部分和小数部分
*/
void PrintFloat(float num, int decimalPlaces) {
    char str[24]; // 增加数组大小以防止溢出
    int intPart = (int)num; // 整数部分
    float decPart = num - (float)intPart; // 小数部分
    int len = 0;

    // 处理整数部分
    if (intPart < 0) {
        str[len++] = '-';
        intPart = -intPart;
    }
    char intStr[12];
    int intLen = 0;
    while (intPart > 0) {
        intStr[intLen++] = (intPart % 10) + '0';
        intPart /= 10;
    }
    if (intLen == 0) {
        intStr[intLen++] = '0';
    }
    int i;
    for (i = intLen - 1; i >= 0; i--) {
        str[len++] = intStr[i];
    }

    // 处理小数部分
    if (decimalPlaces > 0) {
        str[len++] = '.';
        int decimalLen = 0;
        while (decimalPlaces-- > 0) {
            decPart *= 10;
            int digit = (int)decPart;
            str[len++] = digit + '0';
            decPart -= digit;
            decimalLen++;
        }
        // 确保小数部分的末尾零也被打印出来
        while (decimalLen < decimalPlaces) {
            str[len++] = '0';
            decimalLen++;
        }
    }

    str[len] = '\0'; // 字符串结束符
    UARTSendString((uint8_t *)str);
}


/*
* @fn:     void InitADC(void)
* @brief:  初始化ADC模块
* @para:   无
* @return: 无
* @comment:无
*/
void InitADC(uint8_t mode) {
    ADC10CTL0 &= ~ENC; // 确保关闭ADC以便配置

    if (mode == TEMPERATURE) {
        ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
        ADC10CTL1 = INCH_10 + ADC10DIV_3;
        __delay_cycles(1000); // 等待参考电压稳定
    } 
    else if (mode == VOLTAGE) {
        ADC10CTL0 = SREF_0 + ADC10SHT_2 + ADC10ON + ADC10IE;
        ADC10CTL1 = INCH_4 + ADC10DIV_0 + CONSEQ_2;
        ADC10AE0 |= BIT4;
        
        ADC10DTC1 = 50; // 设置DTC传输次数
    }

    ADC10SA = (uint16_t)adcbuff;
    ADC10CTL0 |= ENC;
}

/*
* @fn:     uint16_t GetADCValue(void)
* @brief:  获取ADC值
* @para:   无
* @return: ADC值
* @comment:启动ADC转换并等待完成，返回ADC转换结果
*/
uint16_t GetADCValue(void) {
    ADC10CTL0 |= ADC10SC | ENC;
    while (ADC10CTL1 & ADC10BUSY);
    return ADC10MEM;
}
uint8_t flag = 0;

/*
* @fn:     void StartADCConvert(void)
* @brief:  启动ADC转换
* @para:   none
* @return: none
* @comment:启动ADC转换并等待完成
*/
void StartADCConvert(void) {
    // 启动ADC转换
    ADC10CTL0 |= ADC10SC | ENC;
    // 等待转换完成
    while (ADC10CTL1 & ADC10BUSY);
}

void DelaySeconds(unsigned int seconds) {
    // 使用Timer A0代替Timer A1
    TA0CTL = TASSEL_2 + MC_1 + ID_3; // SMCLK, Up mode, divide by 8
    TA0CCR0 = 125000 - 1; // 1MHz/8 = 125kHz, 需要125000个周期产生1秒
    
    while (seconds--) {
        TA0CTL &= ~TAIFG; // 清除标志位
        while (!(TA0CTL & TAIFG)); // 等待标志位置位
    }
    
    TA0CTL = MC_0; // 停止定时器
}


/*
* @fn:     uint16_t Max(uint16_t *numptr, uint16_t num)
* @brief:  找出数组中的最大值
* @para:   numptr:数组指针，num:数组长度
* @return: 数组中的最大值
* @comment:遍历数组找出最大值
*/
uint16_t Max(uint16_t *numptr, uint16_t num) {
    uint16_t cnt = 0;
    uint16_t max = numptr[0];
    for (cnt = 1; cnt < num; cnt++) {
        // 如果当前值大于已知的最大值，则更新最大值
        if (numptr[cnt] > max) {
            max = numptr[cnt];
        }
    }
    return max;
}

/*
* @fn:     uint16_t Min(uint16_t *numptr, uint16_t num)
* @brief:  找出数组中的最小值
* @para:   numptr:数组指针，num:数组长度
* @return: 数组中的最小值
* @comment:遍历数组找出最小值
*/
uint16_t Min(uint16_t *numptr, uint16_t num) {
    uint16_t cnt = 0;
    uint16_t min = numptr[0];
    for (cnt = 1; cnt < num; cnt++) {
        if (numptr[cnt] < min) {
            min = numptr[cnt];
        }
    }
    return min;
}


/*
* @fn:     uint16_t Average(uint16_t *datptr, uint16_t num)
* @brief:  计算数组的平均值
* @para:   datptr:数组指针，num:数组长度
* @return: 数组的平均值
* @comment:遍历数组计算平均值
*/
uint16_t Average(uint16_t *datptr, uint16_t num) {
    uint32_t sum = 0;
    uint16_t cnt = 0;
    for (cnt = 0; cnt < num; cnt++) {
        sum += datptr[cnt];
    }
    return (uint16_t)(sum / num);
}

void SendVoltageInfo(void) {
    InitADC(VOLTAGE);
    uint8_t cnt = 0;
    while (1) {
        // 循环启动50次ADC转换
        for (cnt = 0; cnt < 50; cnt++) {
            StartADCConvert();
        }
        // 计算最大值和最小值
        maxval = Max(adcbuff, 50);
        minval = Min(adcbuff, 50);
        // 计算峰峰值
        vpp = maxval - minval;
        // 通过UART发送最大值
        UARTSendString("Max: ");
        PrintNumber(maxval);
        UARTSendString("\n");

        // 通过UART发送最小值
        UARTSendString("Min: ");
        PrintNumber(minval);
        UARTSendString("\n");

        // 通过UART发送平均值
        UARTSendString("Average: ");
        PrintFloat((float)Average(adcbuff, 50) * 2.5 / 1023, 3);
        UARTSendString("\n");

        // 延时，等待下一次转换
        __delay_cycles(1000000);
    }
}

void SendTemperatureInfo(void) {
    InitADC(TEMPERATURE);
    P1DIR |= BIT6;

    // 配置Timer1用于周期性测量
    TA1CTL = TASSEL_2 + MC_1 + ID_3; // SMCLK, Up mode, divide by 8
    TA1CCR0 = 62500 - 1; // 500ms @ 1MHz/8
    TA1CTL |= TAIE;
    
    while(1) {
        if(flag) {
            flag = 0;
            P1OUT ^= BIT6;
            
            // 进行单次转换
            ADC10CTL0 |= ADC10SC;
            while (ADC10CTL1 & ADC10BUSY);
            
            temp = ADC10MEM;
            IntDegC = ((temp - 673) * 423) / 1024.0;
            IntDegF = IntDegC * 9.0/5.0 + 32.0;

            UARTSendString("Temperature: ");
            PrintFloat(IntDegC, 2);
            UARTSendString(" C, ");
            PrintFloat(IntDegF, 2);
            UARTSendString(" F.");
            UARTSendString("\n");
        }
        __bis_SR_register(LPM0_bits + GIE);
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    InitSystemClock();
    InitUART();
    
    unsigned char running = 1; // 添加运行状态标志
    
    __bis_SR_register(GIE);  // Enable global interrupts

    while (1) {
        __bis_SR_register(LPM0_bits + GIE);  // Enter LPM0, enable interrupts
        
        if (command == 'S') {
            running = 0;  // 收到终止命令时设置标志为0
        }
        
        if (running) {  // 只在running为1时执行采集
            switch (command) {
                case 'V':
                    SendVoltageInfo();
                    break;
                case 'T':
                    SendTemperatureInfo();
                    break;
                default:
                    break;
            }
        }
        
        command = 0;  // Reset command
    }

    return 0;
}

// 接收pc机命令的中断向量
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
    command = UCA0RXBUF;  // Store received command
    P1OUT ^= BIT0;
    __bic_SR_register_on_exit(LPM0_bits);  // Wake up from LPM0
}


#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    __bic_SR_register_on_exit(CPUOFF);
}

// Timer1 A0 interrupt service routine
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void) {
    switch(TA1IV) {
        case TA1IV_TAIFG:
            flag = 1;
            __bic_SR_register_on_exit(LPM0_bits); // 退出低功耗模式
            break;
    }
}