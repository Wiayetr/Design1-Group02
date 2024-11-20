#include "msp430g2553.h" // ����msp430g2553��Ƭ����ͷ�ļ�
#include "stdlib.h"     // ������׼��ͷ�ļ�

#define GATE_0_2v  56       // ����һ���꣬���ڽ�0.2V��ѹת��ΪADC����ֵ����3.6VΪ�ο���ѹ

unsigned int Num=0;       // ����һ���޷������ͱ���Num�����ڼ���
unsigned int First_votage,votage_temp; // ���������޷������ͱ��������ڴ洢��ѹֵ

void main(void)
{
  unsigned int i=0,j=0;    // ���������޷������ͱ���i��j������ѭ������
  int *Flash_ptr = (int *) 0xd000; // ��Flash�ĵ�ַ0xd000ת��Ϊ����ָ��
  unsigned int flash_time; // ����һ���޷������ͱ���flash_time�����ڼ�ʱ

  WDTCTL = WDTPW + WDTHOLD; // ֹͣ���Ź���ʱ��
  ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ����ADC10������ADC�������ж�
  ADC10CTL1 = INCH_1;       // ����ADC����ͨ��ΪA1
  ADC10AE0 |= 0x02;         // ����PA.1��ΪADCͨ��
  P1DIR |= 0x01;            // ����P1.0Ϊ�������
  P1DIR &= ~BIT3;           // ����P1.3Ϊ���뷽��
  P1OUT |= BIT3;            // ��P1.3�øߵ�ƽ
  P1REN |= BIT3;            // ����P1.3�ĵ���
  First_votage=0;           // ��ʼ��First_votageΪ0
  for(i=0;i<32;i++)         // �ɼ�32��ADCֵ���ۼӵ�First_votage��
  {
      ADC10CTL0 |= ENC + ADC10SC; // ��ʼADC������ת��
      __bis_SR_register(CPUOFF + GIE); // ����͹���ģʽ0���ȴ�ADC�ж�
      First_votage += ADC10MEM;      // �ۼ�ADC���
  }
  First_votage = First_votage >> 5; // ����32�β�����ƽ��ֵ

  P1OUT &= ~BIT0;                   // ��P1.0�õ͵�ƽ��LEDϨ��
  P1OUT |= BIT0;                    // ��P1.0�øߵ�ƽ��LED����
  flash_time=0;                     // ��ʼ��flash_timeΪ0
  do
  {
     if(!(P1IN & BIT3))             // ���P1.3Ϊ�͵�ƽ���������£���������ѭ��
         break;
     flash_time++;                  // ��ʱ������
     if(flash_time == 300)          // �����ʱ���ﵽ300�������LED
         P1OUT |= BIT0;
     if(flash_time == 600)         // �����ʱ���ﵽ600����Ϩ��LED�����ü�ʱ��
     {    P1OUT &= ~BIT0;  flash_time=0;   }

     votage_temp=0;                 // ��ʼ��votage_tempΪ0
     for(i=0;i<32;i++)             // �ٴβɼ�32��ADCֵ���ۼӵ�votage_temp��
     {
        ADC10CTL0 |= ENC + ADC10SC; // ��ʼADC������ת��
        __bis_SR_register(CPUOFF + GIE); // ����͹���ģʽ0���ȴ�ADC�ж�
        votage_temp += ADC10MEM;    // �ۼ�ADC���
     }
     votage_temp = votage_temp >> 5; // ����32�β�����ƽ��ֵ
     votage_temp = abs(votage_temp-First_votage); // �������β����ĵ�ѹ��
  }while(votage_temp < GATE_0_2v);  // �����ѹ�仯����0.2V����������ļ�¼���룡

  flash_init();                     // ��ʼ��Flash
  erase_en_write_D000();             // ������д���ַ0xd000�������ݣ����5120��������

  P1OUT |= BIT0;                    // ����LED
  for (;;)                          // ѭ��ֱ���洢��4096�ֽڣ�2048�����ʣ���
  {
    ADC10CTL0 |= ENC + ADC10SC;     // ��ʼADC������ת��
    __bis_SR_register(CPUOFF + GIE); // ����͹���ģʽ0���ȴ�ADC�ж�

    *Flash_ptr++ = ADC10MEM;        // ��ADC���ݱ��浽Flash��0xd000��
    j++;
    if(j >=(2048+2048))             // ����Ƿ��Ѿ��洢���㹻������
        break;
  }
  close_write_flash();               // �ر�Flashд��
  P1OUT = 0;                         // �ر�����P1�˿ڵ����

  while(1);                          // ������ѭ��
}

// ADC10�жϷ������
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  {__bic_SR_register_on_exit(CPUOFF);}  // ���CPUOFFλ���ӵ͹���ģʽ�˳� }
  Num++;                             // Num����������
}
