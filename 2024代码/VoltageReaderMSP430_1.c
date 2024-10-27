#include <msp430.h>
#include "stdint.h"

/*
 * @fn:     void InitSystemClock(void)
 * @brief:  初始化系统时钟
 * @para:   none
 * @return: none
 * @comment:设置DCO和BCSCTL寄存器，使系统时钟为1MHz
 */
void InitSystemClock(void) {
    DCOCTL = CALDCO_1MHZ;  // 校准并设置DCO为1MHz
    BCSCTL1 = CALBC1_1MHZ; // 校准并设置BCSCTL1为1MHz
    BCSCTL2 &= ~SELS;     // 选择DCO作为时钟源
    BCSCTL2 &= ~(DIVS0 | DIVS1); // 不对时钟进行分频
}

/*
 * @fn:     void InitUART(void)
 * @brief:  初始化UART模块
 * @para:   none
 * @return: none
 * @comment:配置USCI_A0作为UART模式，设置波特率为9600
 */
void InitUART(void) {
    UCA0CTL1 |= UCSWRST; // 复位USCI_A0模块

    UCA0CTL0 &= ~UCSYNC; // 异步模式
    UCA0CTL1 |= UCSSEL_2; // 选择SMCLK作为时钟源
    UCA0BR0 = 0x68;       // 设置波特率生成器参数
    UCA0BR1 = 0x00;       // 设置波特率生成器参数
    UCA0MCTL = UCBRS_0 + UCBRF_0; // 设置波特率参数
    P1SEL |= BIT1 + BIT2; // 选择P1.1和P1.2作为UART的TXD和RXD
    P1SEL2 |= BIT1 + BIT2; // 启用P1.1和P1.2的第二功能

    UCA0CTL1 &= ~UCSWRST; // 启用USCI_A0模块
}


/*
 * @fn:     void UARTSendByte(uint8_t byte)
 * @brief:  通过UART发送单个字节
 * @para:   byte:要发送的字节
 * @return: none
 * @comment:等待UART发送缓冲区为空，然后发送一个字节
 */
void UARTSendByte(uint8_t byte) {
    while (UCA0STAT & UCBUSY);
    UCA0TXBUF = byte;
}

/*
 * @fn:     void UARTSendString(uint8_t *pbuff)
 * @brief:  通过UART发送字符串
 * @para:   pbuff:指向要发送字符串的指针
 * @return: none
 * @comment:发送字符串直到字符串结束符'\0'
 */
void UARTSendString(uint8_t *pbuff) {
    while (*pbuff) {
        UARTSendByte(*pbuff++);
    }
}

/*
 * @fn:     void PrintNumber(int num)
 * @brief:  通过UART发送整数
 * @para:   num:要发送的整数
 * @return: none
 * @comment:将整数转换为字符串并通过UART发送，包括负数处理
 */
void PrintNumber(int num) {
    char buffer[12];
    int i = 0;

    if (num < 0) {
        buffer[i++] = '-';
        num = -num;
    }

    int len = 0;
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
        len++;
    }
    if (len == 0) {
        buffer[i++] = '0';
    }

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    buffer[i] = '\0';

    UARTSendString((uint8_t *)buffer);
}

/*
 * @fn:     void PrintFloat(float num, int decimalPlaces)
 * @brief:  通过UART发送浮点数
 * @para:   num:要发送的浮点数
 *         decimalPlaces:小数点后的位数
 * @return: none
 * @comment:将浮点数转换为字符串并通过UART发送，包括整数部分和小数部分
 */
void PrintFloat(float num, int decimalPlaces) {
    char str[12];
    int intPart = (int)num;
    float decPart = num - (float)intPart;
    int decMultiplier = 10;
    int i;

    int len = 0;
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
    for (i = intLen - 1; i >= 0; i--) {
        str[len++] = intStr[i];
    }

    if (decimalPlaces > 0) {
        str[len++] = '.';
        while (decimalPlaces-- > 0 && decPart > 0) {
            decPart *= decMultiplier;
            int digit = (int)decPart;
            str[len++] = digit + '0';
            decPart -= digit;
        }
    }

    str[len] = '\0';
    UARTSendString(str);
}

/*
 * @fn:     void InitADC(void)
 * @brief:  初始化ADC模块
 * @para:   none
 * @return: none
 * @comment:配置ADC10寄存器，选择输入通道和参考电压，开启ADC
 */
void InitADC(void) {
    ADC10CTL1 |= ADC10SSEL_2; // 选择SMCLK作为ADC时钟源
    ADC10CTL1 |= ADC10DIV_0;  // 设置ADC时钟分频
    ADC10CTL0 |= SREF_1;     // 设置参考电压为1.5V
    ADC10CTL0 |= ADC10SHT_3; // 设置采样保持时间
    ADC10CTL0 &= ~ADC10SR;   // 设置ADC分辨率为10位
    ADC10CTL0 |= REF2_5V;    // 设置参考电压为2.5V
    ADC10CTL0 |= REFON;      // 启用参考电压
    ADC10CTL1 |= INCH_0;     // 选择ADC通道0
    ADC10AE0 |= 0x0001;      // 启用通道0的模拟输入
    ADC10CTL0 |= ADC10ON;    // 启用ADC
}

/*
 * @fn:     uint16_t GetADCValue(void)
 * @brief:  获取ADC转换结果
 * @para:   none
 * @return: ADC转换结果
 * @comment:启动ADC转换，等待转换完成，并返回10位的ADC值
 */
uint16_t GetADCValue(void) {
    ADC10CTL0 |= ADC10SC | ENC; // 启动ADC转换并启用转换序列
    while (ADC10CTL1 & ADC10BUSY); // 等待转换完成
    return ADC10MEM; // 返回ADC转换结果
}

int main(void) {
    float voltage = 0;
    uint16_t adcvalue = 0;
    WDTCTL = WDTPW | WDTHOLD;
    InitSystemClock();
    InitUART();
    InitADC();
    while (1) {
        adcvalue = GetADCValue();
        voltage = adcvalue * 2.5 / 1023;
        UARTSendString("ADC10转换结果为: ");
        PrintNumber(adcvalue);
        UARTSendString("\r\n");
        UARTSendString("相应电压值为: ");
        PrintFloat(voltage, 3);
        UARTSendString("\r\n");
        __delay_cycles(500000);
    }
    return 0;
}
