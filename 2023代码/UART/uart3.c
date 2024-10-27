#include <msp430.h> 
#include "stdint.h"

void InitSystemClock(void)
{
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

void InitUART(void)
{
    UCA0CTL1 |= UCSWRST;

    UCA0CTL0 &= ~UCSYNC;

    UCA0CTL1 |= UCSSEL1;

    UCA0BR0 = 0x68;
    UCA0BR1 = 0x00;
    UCA0MCTL = 1 << 1;
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    UCA0CTL1 &= ~UCSWRST;
}

void UARTSendString(uint8_t *pbuff,uint8_t num)
{
	uint8_t cnt = 0;
	for(cnt = 0;cnt < num;cnt ++)
	{
		while(UCA0STAT & UCBUSY);
		//__delay_cycles(5000);
		UCA0TXBUF = *(pbuff + cnt);
	}
}

void PrintNumber(uint16_t num)
{
	uint8_t buff[6] = {0,0,0,0,0,'\n'};
	uint8_t cnt = 0;
	for(cnt = 0;cnt < 5;cnt ++)
	{
		buff[4 - cnt] = (uint8_t)(num % 10 + '0');
		num /= 10;
	}
	UARTSendString(buff,6);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    InitSystemClock();
    InitUART();

    while(1)
    {
    	PrintNumber(32415);
    	__delay_cycles(500000);
    }
	return 0;
}
