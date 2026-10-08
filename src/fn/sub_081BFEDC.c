#include "global.h"
void sub_082195E0(u8 *);
void sub_0805E468(u8 *);
void sub_0805E2F0(u8 *);
void sub_081BFEDC(u8 *p)
{
    u8 *q = p + 0x18;
    s32 i = 0x37;
    do {
        if (q[4]) sub_082195E0(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    sub_0805E468(p + 0x1558);
    sub_0805E2F0(p + 0x164c);
}
