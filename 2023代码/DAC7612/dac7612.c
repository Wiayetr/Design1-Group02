#include <msp430.h> 
#include <stdint.h>

#define		DAC_A_OUT		0
#define		DAC_B_OUT		1
#define		DAC_AB_OUT		2

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
	UCA0CTL0 = UCMSB|UCMST|UCSYNC|UCCKPL;
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

void InitDAC7612(void)
{
	P1DIR |= BIT5 + BIT7;
	P1OUT |= BIT5 + BIT7;
}

void WriteDAC7612(uint16_t data,uint8_t chn)
{
	uint16_t temp = 0;
	temp = data & 0xFFF;
	switch(chn)
	{
	case DAC_A_OUT:
		temp |= 0x2000;
		break;
	case DAC_B_OUT:
		temp |= 0x3000;
		break;
	case DAC_AB_OUT:
		temp &= ~0x2000;
		break;
	default:
		break;
	}
	P1OUT &= ~BIT5;
	SpiBusWrite((uint8_t)(temp >> 8));
	SpiBusWrite((uint8_t)(temp & 0x00FF));
	P1OUT |= BIT5;
	P1OUT &= ~BIT7;
	__delay_cycles(1000);
	P1OUT |= BIT7;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	
    InitSystemClock();
    InitSpiBus();
    InitDAC7612();
    WriteDAC7612(2048,DAC_A_OUT);
    while(1)
    {
    	__delay_cycles(1000000);
    }
	return 0;
}
