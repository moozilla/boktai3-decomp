#include "global.h"

void sub_08224524(void);
void sub_082455DC(void *);

void sub_08224BA0(u32 a) {
    u8 *p = (u8 *)0x03005390;
    *(u32 *)(p + 0x44) = a;
    sub_082455DC(sub_08224524);
}
