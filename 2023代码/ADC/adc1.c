void InitADC(void) {
    ADC10CTL1 |= ADC10SSEL_2;
    ADC10CTL1 |= ADC10DIV_0;
    ADC10CTL0 |= SREF_1;
    ADC10CTL0 |= ADC10SHT_3;
    ADC10CTL0 &= ~ADC10SR;
    ADC10CTL0 |= REF2_5V;
    ADC10CTL0 |= REFON;
    ADC10CTL1 |= INCH_0;
    ADC10AE0 |= 0x0001;
    ADC10CTL0 |= ADC10ON;
}

uint16_t GetADCValue(void) {
    ADC10CTL0 |= ADC10SC | ENC; 
    while (ADC10CTL1 & ADC10BUSY); 
    return ADC10MEM; 
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
        UARTSendString("ADC10转换结果为：", 17);
        PrintNumber(adcvalue); 
        UARTSendString("相应电压值为：", 14); 
        PrintFloat(voltage); 
        __delay_cycles(300000); 
    }

    return 0;
}
