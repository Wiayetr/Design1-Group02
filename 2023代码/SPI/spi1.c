#include <msp430.h> 
#include <stdint.h>

volatile uint8_t rx_data;

void InitSystemClock(void)
{
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL2 &= ~SELS;
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

void InitSpiBus(void)
{
	P1SEL |= BIT1 | BIT2 | BIT4;
	P1SEL2 |= BIT1 | BIT2 | BIT4;
	UCA0CTL1 |= UCSWRST|UCSSEL_2;
	UCA0CTL0 = UCMSB|UCMST|UCSYNC;
	UCA0BR0 = 1;
	UCA0BR1 = 0;
	UCA0CTL1 &= ~UCSWRST;
}

void SpiBusWrite(uint8_t data)
{
	UCA0TXBUF = data;
	while(!(IFG2 & UCA0TXIFG));
}

uint8_t SpiBusRead(void)
{
	while(!(IFG2 & UCA0RXIFG));
	return UCA0RXBUF;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    InitSystemClock();
    InitSpiBus();
    SpiBusWrite(0xAA);
    rx_data = SpiBusRead();
    while(1)
    {
    	__delay_cycles(1000000);
    }
	return 0;
}
