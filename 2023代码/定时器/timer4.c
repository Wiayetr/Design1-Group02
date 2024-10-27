#include <msp430.h> 
#include "stdint.h"
#include <stdio.h>
uint32_t timestamp = 0;
uint16_t capvalue_1 = 0;
uint16_t capvalue_2 = 0;
uint32_t timestamp_1 = 0;
uint32_t timestamp_2 = 0;
uint32_t totaltime = 0;
float freq = 0;

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

void UARTPrint(uint8_t *pbuff)
{
	uint8_t cnt = 0;
	while(*(pbuff + cnt) != '\0')
	{
		while(UCA0STAT & UCBUSY);
		UCA0TXBUF = *(pbuff + cnt);
		cnt ++;
	}
}

void PrintFreq(float freq)
{
	uint32_t temp = (uint32_t)(freq * 1000);
	uint8_t charbuff[] = {0,0,0,0,0,0,0,0,0};
	int8_t cnt = 0;
	for(cnt = 8;cnt >= 0;cnt --)
	{
		charbuff[cnt] = (uint8_t)(temp % 10) + '0';
		temp /= 10;
	}
	UARTSendString("Ƶ��Ϊ��",8);
	UARTSendString(charbuff,6);
	UARTSendString(".",1);
	UARTSendString(charbuff + 6,3);
	UARTSendString("Hz",2);
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	
    InitSystemClock();
    InitUART();

    TA1CTL |= TASSEL_2;
    TA1CTL |= MC_1;
    TA1CCR0 = 49999;
    TA1CTL |= TAIE;

    TA1CCTL2 |= CAP;
    TA1CCTL2 |= CM0;
    TA1CCTL2 |= CCIS0;
    P2SEL |= BIT5;
    TA1CCTL2 |= CCIE;

    TA0CTL |= TASSEL1;
    TA0CTL |= MC0|MC1;
    TA0CCR0 = 0x0AAA;
    TA0CCR1 = 0x0555;
    TA0CCTL0 &= ~CAP;
    TA0CCTL1 &= ~CAP;
    TA0CCTL1 |= OUTMOD_6;
    P1SEL |= BIT6;
    P1DIR |= BIT6;

    __bis_SR_register(GIE);
    while(1)
    {
    	__delay_cycles(500000);
    	freq = (float)(1000000.0) / totaltime;
    	PrintFreq(freq);
    }
	return 0;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void Time_Tick(void)
{
	static uint8_t cnt = 0;
	__bis_SR_register(GIE);
	switch(TA1IV)
	{
	case 0x02:
		break;
	case 0x04:
		if(cnt == 0)
		{
			capvalue_1 = TA1CCR2;
			timestamp_1 = timestamp;
			cnt ++;
		}
		else
		{
			capvalue_2 = TA1CCR2;
			timestamp_2 = timestamp;
			cnt = 0;
			totaltime = (timestamp_2 - timestamp_1) * 50000 + capvalue_2 - capvalue_1;
		}
		break;
	case 0x0A:
		timestamp ++;
		break;
	default:
		break;
	}
}
