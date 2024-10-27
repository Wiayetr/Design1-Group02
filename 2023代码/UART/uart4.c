#include <msp430.h> 
#include "stdint.h"
uint8_t combuff[20] = {0};
uint8_t iscomend = 0;

void InitSystemClock(void)
{
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL2 &= ~SELS;
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

    IE2 |= UCA0RXIE;
    IFG2 &= ~UCA0RXIFG;
}

void UARTSendString(uint8_t *pbuff,uint8_t num)
{
	uint8_t cnt = 0;
	for(cnt = 0;cnt < num;cnt ++)
	{
		while(UCA0STAT & UCBUSY);
		UCA0TXBUF = *(pbuff + cnt);
	}
}

void Execute(uint8_t *combuff)
{
	const uint8_t charbuff[5][10] = {"ÍõÁú£¿","Ã×½ùÂ¡£¿","°à³¤£¿","LED1 On!","LED Off!"};
	if(combuff[0] == charbuff[0][0] && combuff[1] == charbuff[0][1])
	{
		UARTSendString("ÃÈÃÈßÕ£¡!(¡Ño¡Ñ)",16);//
	}
	else if(combuff[0] == charbuff[1][0] && combuff[1] == charbuff[1][1])
	{
		UARTSendString("ÎÒÏë³ÔÅëÓãÑç( ¨‹-¨‹ )",21);
	}
	else if(combuff[0] == charbuff[2][0] && combuff[1] == charbuff[2][1])
	{
		UARTSendString("ÍÅÖ§Êé£¡!¨r(¨s-¨t)¨q",20);
	}
	else if(combuff[0] == charbuff[3][0] && combuff[6] == charbuff[3][6])
	{
		UARTSendString("Yes!My Lord!(^^©g",17);
		P1OUT |= BIT0;
	}
	else if(combuff[0] == charbuff[4][0] && combuff[6] == charbuff[4][6])
	{
		UARTSendString("Yes!My Highness!(^^©g",21);
		P1OUT &= ~BIT0;
	}
	else
	{
		UARTSendString("What Are You Fucking Talking!(¨p¨‰Ãó¨‰)Í¹",41);
	}
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    InitSystemClock();
    InitUART();
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    __bis_SR_register(GIE);
    while(1)
    {
    	if(iscomend)
    	{
    		iscomend = 0;
    		Execute(combuff);
    	}
    }
	return 0;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void UART_Receive_ISR(void)
{
	static uint8_t cnt = 0;
	if(IFG2 & UCA0RXIFG)
	{
		IFG2 &= ~UCA0RXIFG;
		combuff[cnt++] = UCA0RXBUF;/
		cnt %= 20;
		if(combuff[cnt - 1] == '\n')
		{
			cnt = 0;
			iscomend = 1;
		}
	}
}
