#include <msp430.h>
#include "stdint.h"

/*
 * @fn:     void InitSystemClock(void)
 * @brief:  ��ʼ��ϵͳʱ��
 * @para:   none
 * @return: none
 * @comment:����DCO��BCSCTL�Ĵ�����ʹϵͳʱ��Ϊ1MHz
 */
void InitSystemClock(void) {
    DCOCTL = CALDCO_1MHZ;  // У׼������DCOΪ1MHz
    BCSCTL1 = CALBC1_1MHZ; // У׼������BCSCTL1Ϊ1MHz
    BCSCTL2 &= ~SELS;     // ѡ��DCO��Ϊʱ��Դ
    BCSCTL2 &= ~(DIVS0 | DIVS1); // ����ʱ�ӽ��з�Ƶ
}

/*
 * @fn:     void InitUART(void)
 * @brief:  ��ʼ��UARTģ��
 * @para:   none
 * @return: none
 * @comment:����USCI_A0��ΪUARTģʽ�����ò�����Ϊ9600
 */
void InitUART(void) {
    UCA0CTL1 |= UCSWRST; // ��λUSCI_A0ģ��

    UCA0CTL0 &= ~UCSYNC; // �첽ģʽ
    UCA0CTL1 |= UCSSEL_2; // ѡ��SMCLK��Ϊʱ��Դ
    UCA0BR0 = 0x68;       // ���ò���������������
    UCA0BR1 = 0x00;       // ���ò���������������
    UCA0MCTL = UCBRS_0 + UCBRF_0; // ���ò����ʲ���
    P1SEL |= BIT1 + BIT2; // ѡ��P1.1��P1.2��ΪUART��TXD��RXD
    P1SEL2 |= BIT1 + BIT2; // ����P1.1��P1.2�ĵڶ�����

    UCA0CTL1 &= ~UCSWRST; // ����USCI_A0ģ��
}


/*
 * @fn:     void UARTSendByte(uint8_t byte)
 * @brief:  ͨ��UART���͵����ֽ�
 * @para:   byte:Ҫ���͵��ֽ�
 * @return: none
 * @comment:�ȴ�UART���ͻ�����Ϊ�գ�Ȼ����һ���ֽ�
 */
void UARTSendByte(uint8_t byte) {
    while (UCA0STAT & UCBUSY);
    UCA0TXBUF = byte;
}

/*
 * @fn:     void UARTSendString(uint8_t *pbuff)
 * @brief:  ͨ��UART�����ַ���
 * @para:   pbuff:ָ��Ҫ�����ַ�����ָ��
 * @return: none
 * @comment:�����ַ���ֱ���ַ���������'\0'
 */
void UARTSendString(uint8_t *pbuff) {
    while (*pbuff) {
        UARTSendByte(*pbuff++);
    }
}

/*
 * @fn:     void PrintNumber(int num)
 * @brief:  ͨ��UART��������
 * @para:   num:Ҫ���͵�����
 * @return: none
 * @comment:������ת��Ϊ�ַ�����ͨ��UART���ͣ�������������
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
 * @brief:  ͨ��UART���͸�����
 * @para:   num:Ҫ���͵ĸ�����
 *         decimalPlaces:С������λ��
 * @return: none
 * @comment:��������ת��Ϊ�ַ�����ͨ��UART���ͣ������������ֺ�С������
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
 * @brief:  ��ʼ��ADCģ��
 * @para:   none
 * @return: none
 * @comment:����ADC10�Ĵ�����ѡ������ͨ���Ͳο���ѹ������ADC
 */
void InitADC(void) {
    ADC10CTL1 |= ADC10SSEL_2; // ѡ��SMCLK��ΪADCʱ��Դ
    ADC10CTL1 |= ADC10DIV_0;  // ����ADCʱ�ӷ�Ƶ
    ADC10CTL0 |= SREF_1;     // ���òο���ѹΪ1.5V
    ADC10CTL0 |= ADC10SHT_3; // ���ò�������ʱ��
    ADC10CTL0 &= ~ADC10SR;   // ����ADC�ֱ���Ϊ10λ
    ADC10CTL0 |= REF2_5V;    // ���òο���ѹΪ2.5V
    ADC10CTL0 |= REFON;      // ���òο���ѹ
    ADC10CTL1 |= INCH_0;     // ѡ��ADCͨ��0
    ADC10AE0 |= 0x0001;      // ����ͨ��0��ģ������
    ADC10CTL0 |= ADC10ON;    // ����ADC
}

/*
 * @fn:     uint16_t GetADCValue(void)
 * @brief:  ��ȡADCת�����
 * @para:   none
 * @return: ADCת�����
 * @comment:����ADCת�����ȴ�ת����ɣ�������10λ��ADCֵ
 */
uint16_t GetADCValue(void) {
    ADC10CTL0 |= ADC10SC | ENC; // ����ADCת��������ת������
    while (ADC10CTL1 & ADC10BUSY); // �ȴ�ת�����
    return ADC10MEM; // ����ADCת�����
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
        UARTSendString("ADC10ת�����Ϊ: ");
        PrintNumber(adcvalue);
        UARTSendString("\r\n");
        UARTSendString("��Ӧ��ѹֵΪ: ");
        PrintFloat(voltage, 3);
        UARTSendString("\r\n");
        __delay_cycles(500000);
    }
    return 0;
}
