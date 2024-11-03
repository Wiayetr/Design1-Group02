#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = CALDCO_8MHZ;
    BCSCTL1 = CALBC1_8MHZ;
    P1DIR |= BIT6;
    P1OUT &= ~BIT6;
    while(1)
    {
    	P1OUT ^= BIT6;
    	__delay_cycles(500000);
    }
	return 0;
}
