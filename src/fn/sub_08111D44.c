#include "global.h"
void sub_0803381C(u32);
void sub_08111D44(u8 *p)
{
    u32 *q = (u32 *)(p + 0x11CC);
    s32 i = 2;
    do {
        sub_0803381C(*q++);
    } while (--i >= 0);
}
