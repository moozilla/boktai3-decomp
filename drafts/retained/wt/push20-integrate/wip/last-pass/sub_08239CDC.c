#include "global.h"
void sub_08239C80(u8 *);
void sub_08239A20(u8 *, s32);
void sub_08239CDC(u8 *p)
{
    u16 *a;
    u8 **link;
    u8 *value;
    sub_08239C80(p);
    *(u16 *)(p + 0xac4) = 0xffff;
    p[0xac8] = 255;
    a = (u16 *)(p + 0xab0);
    *(u16 *)(p + 0xc6) = *a;
    value = p + 0x360;
    *(u8 **)(p + 0xd4) = value;
    link = (u8 **)(p + 0x178);
    *(u16 *)(*link + 6) = *a;
    *(u8 **)(*link + 12) = value;
    sub_08239A20(p, 0);
}
