#include <msp430.h> 

/*
 * main.c
 */
int main(void)
{
	unsigned int cnt = 0;
    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    P1DIR &= ~BIT6;

    TA1CTL |= TASSEL1;
    TA1CTL |= MC0|MC1;
    TA1CCR0 = 0x00FF;
    TA1CCR2 = 0x00FF;
    TA1CCTL0 &= ~CAP;
    TA1CCTL2 &= ~CAP;
    TA1CCTL2 |= OUTMOD_6;
    P2SEL |= BIT5;
    P2DIR |= BIT5;
    while(1)
    {
    	for(cnt = 0;cnt < 0x00FF;cnt ++)
    	{
    		TA1CCR2 = cnt;
    		__delay_cycles(5000);
    	}
    	for(cnt = 0x00FF;cnt > 0;cnt --)
    	{
    		TA1CCR2 = cnt;
    		__delay_cycles(5000);
    	}
    }
	return 0;
}
