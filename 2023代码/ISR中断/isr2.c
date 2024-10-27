#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	
    P1DIR |= BIT6;
    P1OUT &= ~BIT6;
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1IE |= BIT3;
    P1IES |= BIT3;
    P1IFG &= ~BIT3;
    __bis_SR_register(GIE);
    while(1)
    {

    }
	return 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
	if(P1IFG & BIT3)
	{
		P1IFG &= ~BIT3;
		P1OUT ^= BIT6;
	}
}
