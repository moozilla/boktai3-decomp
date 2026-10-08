#include "global.h"

u16 *sub_0805DD18(s32, s32, s32);
void sub_0805DD68(s32 a, s32 b, s32 n)
{
    u16 *p = sub_0805DD18(0, a, b);
    s32 i;
    for (i = n; i > 0; i--)
        *p++ = 0xF020;
}