#include "global.h"
void sub_081BC664(u8 *p)
{
    u32 one = 1;
    u8 *q = p + 0x60;
    s32 i = 0x15;
    do {
        *(u32 *)q |= one;
        q += 0x60;
        i--;
    } while (i >= 0);
    *(u32 *)(p + 0x898) |= 1;
}
