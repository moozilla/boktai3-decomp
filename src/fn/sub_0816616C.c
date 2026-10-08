#include "global.h"
void sub_0803381C(u32);
void sub_0816616C(u8 *p)
{
    u32 *q = (u32 *)(p + 0x48C4);
    s32 i = 7;
    do {
        sub_0803381C(*q++);
        i--;
    } while (i >= 0);
}
