#include "global.h"

void sub_0803DC1C(u8 *a, u8 *e)
{
    s32 *c = (s32 *)(e + 0x178);
    if (*c > 0) {
        *c = *c - 1;
        if (*(s32 *)(e + 0x174) < 0)
            *(u32 *)(a + 0x858) |= 0x40;
        else
            *(u32 *)(a + 0x858) |= 0x80;
    }
}
