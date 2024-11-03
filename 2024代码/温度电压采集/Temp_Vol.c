#include <msp430.h>
#include "stdint.h"

uint16_t adcbuff[50] = {0};
uint16_t maxval = 0;
uint16_t minval = 0;
uint16_t vpp = 0;

long temp; // Store the raw temperature sensor reading
float IntDegF; // Store temperature in Fahrenheit
float IntDegC; // Store temperature in Celsius

void InitSystemClock(void) {
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL2 &= ~SELS;
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

void InitUART(void) {
    UCA0CTL1 |= UCSWRST;
    UCA0CTL0 &= ~UCSYNC;
    UCA0CTL1 |= UCSSEL1;
    UCA0BR0 = 0x68;
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS_0 + UCBRF_0;
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    UCA0CTL1 &= ~UCSWRST;
}

void UARTSendByte(uint8_t byte) {
    while (UCA0STAT & UCBUSY);
    UCA0TXBUF = byte;
}

void UARTSendString(uint8_t *pbuff) {
    while (*pbuff) {
        UARTSendByte(*pbuff++);
    }
}

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

void PrintFloat(float num, int decimalPlaces) {
    char str[24];
    int intPart = (int)num;
    float decPart = num - (float)intPart;
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
    int i;
    for (i = intLen - 1; i >= 0; i--) {
        str[len++] = intStr[i];
    }

    if (decimalPlaces > 0) {
        str[len++] = '.';
        while (decimalPlaces-- > 0) {
            decPart *= 10;
            int digit = (int)decPart;
            str[len++] = digit + '0';
            decPart -= digit;
        }
    }

    str[len] = '\0';
    UARTSendString((uint8_t *)str);
}

void InitADC(void) {
    ADC10CTL1 |= INCH_4; // ADC input channel for voltage
    ADC10CTL0 |= SREF_1 | ADC10SHT_2 | REFON | ADC10ON | ADC10IE; // Set reference, sample time, and enable ADC
}

uint16_t GetADCValue(void) {
    ADC10CTL0 |= ADC10SC | ENC; // Start conversion
    while (ADC10CTL1 & ADC10BUSY);
    return ADC10MEM; // Return ADC value
}

void InitTempSensor(void) {
    ADC10CTL1 |= INCH_10; // Use internal temperature sensor
    ADC10CTL0 |= SREF_1 | ADC10SHT_3 | REFON | ADC10ON | ADC10IE; // Set reference and enable ADC
}

void StartADCConvert(uint8_t channel) {
    ADC10CTL1 = channel; // Set the channel
    GetADCValue(); // Perform the conversion
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    InitSystemClock();
    InitUART();
    InitADC();
    InitTempSensor();

    while (1) {
        // Read voltage
        StartADCConvert(INCH_4);
        uint16_t voltage = GetADCValue();
        // Send voltage value
        UARTSendString((uint8_t *)"Voltage: ");
        PrintFloat((float)voltage * 2.5 / 1023, 3); // Convert to voltage
        UARTSendString((uint8_t *)" V\n");

        // Read temperature
        StartADCConvert(INCH_10);
        temp = GetADCValue();
        IntDegC = (temp - 673) * 423.0 / 1024.0; // Convert ADC value to Celsius
        // Send temperature value
        UARTSendString((uint8_t *)"Temperature: ");
        PrintFloat(IntDegC, 2);
        UARTSendString((uint8_t *)" °„C\n");

        // Delay between readings
        __delay_cycles(1000000); // Adjust delay as necessary
    }
    return 0;
}
