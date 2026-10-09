#include "global.h"
#include "gba/io_reg.h"
extern void (*gUnk_03003A10[])(void);
void sub_082141BC(void);
void sub_08243CDC(void)
{
    u16 ie;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 0;
    *(vu16 *)0x0400010E = 0;
    *(vu16 *)0x0400010C = 0;
    gUnk_03003A10[2] = sub_082141BC;
    *(vu16 *)0x080000C8 = 0;
    REG_IE = ie & 0xFFBF;
    REG_IME = 1;
}
