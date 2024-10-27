#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P1DIR |= BIT6;
    P1OUT &= ~BIT6;
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;
    while(1)
    {
    	if(P1IN & BIT3)
    	{
    		P1OUT &= ~BIT6;
    	}
    	else
    	{
    		P1OUT |= BIT6;
    	}
    }
	return 0;
}
