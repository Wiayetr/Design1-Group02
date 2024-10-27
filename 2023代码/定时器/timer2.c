#include <msp430.h> 
#include "stdint.h"

uint32_t currenttime = 40500;

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

void PrintTime(uint32_t time)
{
	uint8_t charbuff[] = {0,0,':',0,0,':',0,0};
	charbuff[7] = (uint8_t)((time % 60) % 10) + '0';
	charbuff[6] = (uint8_t)((time % 60) / 10) + '0';
	charbuff[4] = (uint8_t)((time % 3600) / 60 % 10) + '0';
	charbuff[3] = (uint8_t)((time % 3600) / 60 / 10) + '0';
	charbuff[1] = (uint8_t)((time / 3600) % 10) + '0';
	charbuff[0] = (uint8_t)(time / 3600 / 10) + '0';
	UARTSendString("��ǰʱ�䣺",10);
	UARTSendString(charbuff,8);
}

uint8_t flag = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	
    InitSystemClock();
    InitUART();
    P1DIR |= BIT6;

    TA1CTL |= TASSEL_2;
    TA1CTL |= MC_1;
    TA1CCR0 = 49999;

    TA1CTL |= TAIE;
    __bis_SR_register(GIE);
    while(1)
    {
    	if(flag == 1)
    	{
    		flag = 0;
    		P1OUT ^= BIT6;
    		PrintTime(currenttime);
    	}
    }
	return 0;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Time_Tick(void)
{
	static uint8_t cnt = 0;
	switch(TA1IV)
	{
	case 0x02:
		break;
	case 0x04:
		break;
	case 0x0A:
		cnt ++;
		if(cnt == 20)
		{
			cnt = 0;
			flag = 1;
			currenttime ++;
			currenttime %= 86400;
		}
		break;
	default:
		break;
	}
}
