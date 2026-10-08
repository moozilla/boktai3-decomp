#include "global.h"

s32 sub_080136B4(u8 *head, u8 *n) {
    u32 z = n[0];
    u8 *t;
    s32 r;
    if (z != 0) {
        r = -1;
    } else {
        *(u32 *)(n + 0x64) = z;
        t = *(u8 **)(head + 0x1C);
        *(u8 **)(n + 0x68) = t;
        if (t) {
            *(u8 **)(t + 0x64) = n;
        }
        *(u8 **)(head + 0x1C) = n;
        n[0] = 1;
        r = 0;
    }
    return r;
}
