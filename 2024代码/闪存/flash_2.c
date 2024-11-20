#include <msp430g2553.h>

char value; // 8λֵ������д���A

// ����ԭ������
void write_SegC(char value);
void copy_C2D(void);

void flash_init(void)
{
  WDTCTL = WDTPW + WDTHOLD; // ֹͣ���Ź���ʱ��
  if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF)
  {
    while(1); // ���У׼�������������򲻼��أ�����CPU!!
  }
  BCSCTL1 = CALBC1_1MHZ; // ����DCOΪ1MHz
  DCOCTL = CALDCO_1MHZ;
  FCTL2 = FWKEY + FSSEL0 + FN1; // MCLK/3����Flashʱ��������
  value = 0; // ��ʼ��ֵ
}

void erase_en_write_D000(void)
{
  char *Flash_ptr; // Flashָ��
  unsigned int i;

  for (i = 0; i < (8 + 8); i++) // 8+8+4 -> 0xd000,0xe000,0xf000-0xf800
  {
    Flash_ptr = (char *)0xd000 + i * 0x200; // ��ʼ��Flashָ��
    FCTL1 = FWKEY + ERASE; // ���ò���λ
    FCTL3 = FWKEY; // �������λ
    *Flash_ptr = 0; // ����д���Բ���Flash��
  }

  FCTL1 = FWKEY + WRT; // ����WRTλ����д����
}

void close_write_flash(void)
{
  FCTL1 = FWKEY; // ���WRTλ
  FCTL3 = FWKEY + LOCK; // ��������λ
}


void write_SegC(char value)
{
  char *Flash_ptr; // Flashָ��
  unsigned int i;

  Flash_ptr = (char *)0xd000; // ��ʼ��Flashָ��
  FCTL1 = FWKEY + ERASE; // ���ò���λ
  FCTL3 = FWKEY; // �������λ
  *Flash_ptr = 0; // ����д���Բ���Flash��

  FCTL1 = FWKEY + WRT; // ����WRTλ����д����

  for (i = 0; i < 64; i++)
  {
    *Flash_ptr++ = value; // ��ֵд��Flash
  }

  for (i = 0; i < 64; i++)
  {
    *Flash_ptr++ = value++; // ��ֵд��Flash������ֵ
  }

  FCTL1 = FWKEY; // ���WRTλ
  FCTL3 = FWKEY + LOCK; // ��������λ
}

void copy_C2D(void)
{
  char *Flash_ptrC; // ��Cָ��
  char *Flash_ptrD; // ��Dָ��
  unsigned int i;

  Flash_ptrC = (char *)0x1040; // ��ʼ��Flash��Cָ��
  Flash_ptrD = (char *)0x1000; // ��ʼ��Flash��Dָ��
  FCTL1 = FWKEY + ERASE; // ���ò���λ
  FCTL3 = FWKEY; // �������λ
  *Flash_ptrD = 0; // ����д���Բ���Flash��D
  FCTL1 = FWKEY + WRT; // ����WRTλ����д����

  for (i = 0; i < 64; i++)
  {
    *Flash_ptrD++ = *Flash_ptrC++; // ����C��ֵ���Ƶ���D
  }

  FCTL1 = FWKEY; // ���WRTλ
  FCTL3 = FWKEY + LOCK; // ��������λ
}
