#include "global.h"

void sub_08224524(void);
void rfu_setMSCCallback(void *);

void sub_08224BA0(u32 a)
{
    u8 *p = (u8 *)0x03005390;
    *(u32 *)(p + 0x44) = a;
    rfu_setMSCCallback(sub_08224524);
}
