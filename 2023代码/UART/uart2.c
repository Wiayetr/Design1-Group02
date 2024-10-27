#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL2 &= ~SELS;
    BCSCTL2 &= ~(DIVS0 | DIVS1);

    UCA0CTL1 |= UCSWRST;

    UCA0CTL0 &= ~UCSYNC;

    UCA0CTL1 |= UCSSEL1;

    UCA0BR0 = 0x68;
    UCA0BR1 = 0x00;
    UCA0MCTL = 1 << 1;
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    UCA0CTL1 &= ~UCSWRST;

    while(1)
    {
    	UCA0TXBUF = 0xAA;
    	__delay_cycles(500000);
    }
	return 0;
}
