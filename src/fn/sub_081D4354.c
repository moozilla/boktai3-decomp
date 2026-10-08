#include "global.h"
void sub_081D42E4(u8 *, u32, u32);
void sub_081D4354(u8 *p)
{
    u32 v = *(u32 *)(p + 0x20);
    if (v > 0x1f) v = 0x40 - v;
    sub_081D42E4(p, v, 5);
    *(u32 *)(p + 0x20) = (*(u32 *)(p + 0x20) + 1) & 0x3f;
}
