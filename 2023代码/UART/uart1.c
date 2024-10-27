void USART0_Config(void) {
    P3SEL |= BIT3 + BIT4; 
    UCA0CTL1 |= UCSWRST; 
    UCA0CTL1 |= UCSSEL__SMCLK; 
    UCA0BR0 = 0x22; 
    UCA0BR1 = 0x00;
    UCA0MCTL |= UCBRS_6 + UCBRF_0;
    UCA0CTL1 &= ~UCSWRST; 
    UCA0IE |= UCRXIE; 
    UCA0IFG &= ~UCRXIFG; 
}

void send0_buf(unsigned char *ptr) {
    while (*ptr != '\0') {
        while (!(UCA0IFG & UCTXIFG)); 
        UCA0TXBUF = *ptr; 
        ptr++;
        delay_ms(10); 
    }
}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR() {
    static int i = 0;
    if (UCA0IV == 0x02) { 
        buff[i] = UCA0RXBUF;
        if (buff[0] != 0x55) 
            i = -1;
        if (i == 1 && (buff[1] != 0x56)) 
            i = -1;
        if (i == 9) {
            for (i = 0; i < 8; i++) {
                Di[i] = buff[i + 2]; 
            }
            i = -1;
        }
        UCA0IFG &= ~UCRXIFG;
        i++;
    }
}
