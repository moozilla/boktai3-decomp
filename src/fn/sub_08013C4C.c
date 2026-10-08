#include "global.h"

s32 sub_08013C4C(u8 *head, u8 *n) {
    u32 z = n[2];
    u8 *t;
    s32 r;
    if (z != 0) {
        r = -1;
    } else {
        *(u32 *)(n + 0x4C) = z;
        t = *(u8 **)(head + 0x18);
        *(u8 **)(n + 0x50) = t;
        if (t) {
            *(u8 **)(t + 0x4C) = n;
        }
        *(u8 **)(head + 0x18) = n;
        n[2] = 1;
        r = 0;
    }
    return r;
}
