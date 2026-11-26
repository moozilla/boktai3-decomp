#include "global.h"
void sub_0821710C(s32 pal, s32 idx, u16 color)
{
    ((u16 *)0x5000200)[pal * 16 + idx] = color;
}
