#include "MSP430.h"
#include "IN430.h"

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; 

    P1DIR |= BIT0; 
    P1OUT &= ~BIT0; 

    P1DIR &= ~BIT1; 

    P1REN |= BIT1;
    P1OUT |= BIT1;

    P1IE |= BIT1;

    P1IES |= BIT1;

    P1IFG &= ~BIT1;

    _BIS_SR(GIE);

    while (1) {
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR() {
    if (P1IFG & BIT1) { 
        P1IFG &= ~BIT1; 
        P1OUT ^= BIT0; 
    }
}
