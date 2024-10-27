#include <msp430.h> 
#include <stdint.h>

volatile uint8_t master_receive_data;
volatile uint8_t slave_receive_data;

void InitSystemClock(void)
{
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL2 &= ~SELS;
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

void InitSpiBusA0(void)
{
	P1SEL |= BIT1 | BIT2 | BIT4;
	P1SEL2 |= BIT1 | BIT2 | BIT4;
	UCA0CTL1 |= UCSWRST|UCSSEL_2;
	UCA0CTL0 = UCMSB|UCMST|UCSYNC;
	UCA0BR0 = 1;
	UCA0BR1 = 0;
	UCA0CTL1 &= ~UCSWRST;
}

void InitSpiBusB0(void)
{
	P1SEL |= BIT5 | BIT6 | BIT7;
	P1SEL2 |= BIT5 | BIT6 | BIT7;
	UCB0CTL0 = UCMSB|UCSYNC;
	UCB0CTL1 &= ~UCSWRST;
}

void SpiBusMatserWrite(uint8_t data)
{
	UCA0TXBUF = data;
	while(!(IFG2 & UCA0TXIFG));
}

uint8_t SpiBusMasterRead(void)
{
	IFG2 &= ~UCA0RXIFG;
	UCA0TXBUF = 0xFF;
	while(!(IFG2 & UCA0RXIFG));
	return UCA0RXBUF;
}

void SpiBusSlaveWrite(uint8_t data)
{
	UCB0TXBUF = data;
}

uint8_t SpiBusSlaveRead(void)
{
	while(!(IFG2 & UCB0RXIFG));
	return UCB0RXBUF;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    InitSystemClock();
    InitSpiBusA0();
    InitSpiBusB0();

    SpiBusMatserWrite(0xAA);
    slave_receive_data = SpiBusSlaveRead();
    SpiBusSlaveWrite(0x55);
    master_receive_data = SpiBusMasterRead();

    while(1)
    {
    	__delay_cycles(1000000);
    }
	return 0;
}
