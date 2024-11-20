#include <msp430g2553.h>

char value; // 8位值，用于写入段A

// 函数原型声明
void write_SegC(char value);
void copy_C2D(void);

void flash_init(void)
{
  WDTCTL = WDTPW + WDTHOLD; // 停止看门狗定时器
  if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF)
  {
    while(1); // 如果校准常数被擦除，则不加载，陷阱CPU!!
  }
  BCSCTL1 = CALBC1_1MHZ; // 设置DCO为1MHz
  DCOCTL = CALDCO_1MHZ;
  FCTL2 = FWKEY + FSSEL0 + FN1; // MCLK/3用于Flash时序生成器
  value = 0; // 初始化值
}

void erase_en_write_D000(void)
{
  char *Flash_ptr; // Flash指针
  unsigned int i;

  for (i = 0; i < (8 + 8); i++) // 8+8+4 -> 0xd000,0xe000,0xf000-0xf800
  {
    Flash_ptr = (char *)0xd000 + i * 0x200; // 初始化Flash指针
    FCTL1 = FWKEY + ERASE; // 设置擦除位
    FCTL3 = FWKEY; // 清除锁定位
    *Flash_ptr = 0; // 虚拟写入以擦除Flash段
  }

  FCTL1 = FWKEY + WRT; // 设置WRT位进行写操作
}

void close_write_flash(void)
{
  FCTL1 = FWKEY; // 清除WRT位
  FCTL3 = FWKEY + LOCK; // 设置锁定位
}


void write_SegC(char value)
{
  char *Flash_ptr; // Flash指针
  unsigned int i;

  Flash_ptr = (char *)0xd000; // 初始化Flash指针
  FCTL1 = FWKEY + ERASE; // 设置擦除位
  FCTL3 = FWKEY; // 清除锁定位
  *Flash_ptr = 0; // 虚拟写入以擦除Flash段

  FCTL1 = FWKEY + WRT; // 设置WRT位进行写操作

  for (i = 0; i < 64; i++)
  {
    *Flash_ptr++ = value; // 将值写入Flash
  }

  for (i = 0; i < 64; i++)
  {
    *Flash_ptr++ = value++; // 将值写入Flash并递增值
  }

  FCTL1 = FWKEY; // 清除WRT位
  FCTL3 = FWKEY + LOCK; // 设置锁定位
}

void copy_C2D(void)
{
  char *Flash_ptrC; // 段C指针
  char *Flash_ptrD; // 段D指针
  unsigned int i;

  Flash_ptrC = (char *)0x1040; // 初始化Flash段C指针
  Flash_ptrD = (char *)0x1000; // 初始化Flash段D指针
  FCTL1 = FWKEY + ERASE; // 设置擦除位
  FCTL3 = FWKEY; // 清除锁定位
  *Flash_ptrD = 0; // 虚拟写入以擦除Flash段D
  FCTL1 = FWKEY + WRT; // 设置WRT位进行写操作

  for (i = 0; i < 64; i++)
  {
    *Flash_ptrD++ = *Flash_ptrC++; // 将段C的值复制到段D
  }

  FCTL1 = FWKEY; // 清除WRT位
  FCTL3 = FWKEY + LOCK; // 设置锁定位
}
