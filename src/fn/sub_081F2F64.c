#include "global.h"

s32 sub_081F2F64(u8 *p) {
    s32 idx = -1;
    u32 best = 0xFFFFFFFF;
    s32 i = 0;
    u8 *q = p + 0xD24;
    do {
        u32 v = *(u32 *)q;
        if (v <= best) {
            best = v;
            idx = i;
        }
        q += 0x18;
        i++;
    } while (i <= 8);
    return idx;
}
