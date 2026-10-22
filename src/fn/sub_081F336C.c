#include "global.h"

s32 sub_081F336C(u8 *p)
{
    s32 best = -1;
    u32 max = 0;
    s32 i;
    for (i = 0; i <= 3; i++) {
        u8 *e = p + 0xdf0 + i * 24;
        if (*(u32 *)e == 1 && *(u32 *)(e + 0x10) >= max) {
            max = *(u32 *)(e + 0x10);
            best = i;
        }
    }
    return best;
}
