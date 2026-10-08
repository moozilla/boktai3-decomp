#include "global.h"
struct E { u8 f0[8]; u32 fl; u8 f1[0x54]; };
void sub_08165B94(u8 *p, s32 i, u32 m)
{
    struct E *e = (struct E *)(i * 0x60 + (u32)p + 0x1420);
    e->fl &= ~m;
}
