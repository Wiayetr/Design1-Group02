#include <msp430.h> 
#include "stdint.h"

uint16_t adcbuff[50] = {0};

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

void UARTSend(uint8_t val)
{
	while(!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = val;
}

void PrintNumber(uint16_t num)
{
	uint8_t buff[6] = {0,0,0,0,0,'\n'};
	uint8_t cnt = 0;
	for(cnt = 0;cnt < 5;cnt ++)
	{
		buff[4 - cnt] = (uint8_t)(num % 10 + '0');
		num /= 10;
	}
	UARTSendString(buff,6);
}

void PrintFloat(float num)
{
	uint8_t charbuff[] = {0,'.',0,0,0};
	uint16_t temp = (uint16_t)(num * 1000);
	charbuff[0] = (uint8_t)(temp / 1000) + '0';
	charbuff[2] = (uint8_t)((temp % 1000) / 100) + '0';
	charbuff[3] = (uint8_t)((temp % 100) / 10) + '0';
	charbuff[4] = (uint8_t)(temp % 10) + '0';
	UARTSendString(charbuff,5);
}

void InitADCTrigByTimerA(void)
{
	  ADC10CTL1 |= ADC10SSEL_2;
	  ADC10CTL1 |= SHS0;
	  ADC10CTL1 |= CONSEQ1;
	  ADC10CTL0 |= SREF_1;
	  ADC10CTL0 |= ADC10SHT_2;
	  ADC10CTL0 |= ADC10SR;
	  ADC10CTL0 |= REF2_5V;
	  ADC10CTL0 |= REFON;
	  ADC10CTL1 |= INCH_4;
	  ADC10AE0 |= 1 << 4;

	  ADC10DTC0 |= ADC10CT;
	  ADC10DTC1 = 50;
	  ADC10SA = (uint16_t)(adcbuff);

	  ADC10CTL0 |= ADC10ON;

	  ADC10CTL0 |= ENC;
}

void InitTimerA(void)
{
    TA0CTL |= TASSEL1;
    TA0CTL |= MC0|MC1;
    TA0CCR0 = 0x00FF;
    TA0CCR1 = 0x007F;
    TA0CCTL0 &= ~CAP;
    TA0CCTL1 &= ~CAP;
    TA0CCTL1 |= OUTMOD_6;
    P1SEL |= BIT6;
    P1DIR |= BIT6;
}

uint16_t GetADCValue(void)
{
	  ADC10CTL0 |= ADC10SC|ENC;
	  while(ADC10CTL1&ADC10BUSY);
	  return ADC10MEM;
}

void StartADCConvert(void)
{
	  ADC10CTL0 |= ADC10SC|ENC;
	  while(ADC10CTL1&ADC10BUSY);
}

uint16_t Max(uint16_t *numptr,uint16_t num)
{
	uint16_t cnt = 0;
	uint16_t max = 0;
	for(cnt = 0;cnt < num;cnt ++)
	{
		if(numptr[cnt] > max){
			max = numptr[cnt];
		}
	}
	return max;
}

uint16_t Min(uint16_t *numptr,uint16_t num)
{
	uint16_t cnt = 0;
	uint16_t min = 0;
	min = numptr[0];
	for(cnt = 0;cnt < num;cnt ++)
	{
		if(numptr[cnt] < min){
			min = numptr[cnt];
		}
	}
	return min;
}

uint16_t Average(uint16_t *datptr)
{
	uint32_t sum = 0;
	uint8_t cnt = 0;
	for(cnt = 0;cnt < 50;cnt ++)
	{
		sum += *(datptr + cnt);
	}
	return (uint16_t)(sum / 50);
}

int main(void)
{
	uint8_t cnt = 0;
    WDTCTL = WDTPW | WDTHOLD;	
    InitSystemClock();
    InitUART();
    InitADCTrigByTimerA();
    InitTimerA();
    while(1)
    {
#if 1
    	for(cnt = 0;cnt < 50;cnt ++)
    	{
    		PrintNumber(*(adcbuff+cnt));
    	}
    	UARTSendString("ADC Sample Finished!\n",21);

    	__delay_cycles(1000000);
#else
    	__delay_cycles(1000000);
#endif
    }
	return 0;
}
