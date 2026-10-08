#include "global.h"
void sub_0821980C(u8 *, u8 *, u32, s32);
void sub_08165B28(u8 *p, s32 i, u32 c)
{
    u8 *e = (u8 *)(i * 0x60 + (u32)p + 0x1420);
    *(u32 *)(e + 8) &= ~1;
    sub_0821980C((u8 *)(p + (i * 0x60 + 0x1420)), p + 0x64, (u16)c, 0);
}
