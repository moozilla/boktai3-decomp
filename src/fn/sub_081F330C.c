#include "global.h"

s32 sub_081F330C(u8 *p) {
    s32 idx = -1;
    u32 best = 0xFFFFFFFF;
    s32 i = 0;
    u8 *q = p + 0xDF0;
    do {
        if (*(u32 *)q == 1) {
            u32 v = *(u32 *)(q + 0x10);
            if (v <= best) {
                best = v;
                idx = i;
            }
        }
        q += 0x18;
        i++;
    } while (i <= 3);
    return idx;
}
