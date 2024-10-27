#include <msp430.h> 
#include "stdint.h"

int main(void)
{
	uint8_t cnt = 0;
    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    P1DIR |= BIT6;

    TA1CTL |= TASSEL_2;
    TA1CTL |= MC_1;
    TA1CCR0 = 49999;
    while(1)
    {
    	if(TA1CTL & TAIFG)
    	{
    		cnt ++;
    		TA1CTL &= ~TAIFG;
    		if(cnt == 20)
    		{
    			P1OUT ^= BIT6;
    			cnt = 0;
    		}
    	}
    }
	return 0;
}
