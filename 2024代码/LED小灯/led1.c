#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    while(1)
    {

    }
	return 0;
}
