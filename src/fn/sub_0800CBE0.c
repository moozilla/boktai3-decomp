#include "global.h"

u32 sub_0800CBE0(u8 *head, u8 *n) {
    u32 *slot;
    u8 *t;
    *(u32 *)(n + 0xEC) = 0;
    slot = (u32 *)(n + 0xF0);
    *slot = *(u32 *)(head + 0x1C);
    t = (u8 *)*slot;
    if (t) {
        *(u8 **)(t + 0xEC) = n;
    }
    *(u8 **)(head + 0x1C) = n;
    return 0;
}
