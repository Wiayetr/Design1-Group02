#include <msp430.h> 

/*
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P1DIR |= BIT6;
    P1OUT &= ~BIT6;
    while(1)
    {
    	P1OUT ^= BIT6;
    	__delay_cycles(500000);
    }
	return 0;
}
