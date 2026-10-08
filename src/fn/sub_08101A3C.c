#include "global.h"

extern u8 *gUnk_020001AC;

u8 *sub_08101A3C(u32 key) {
    u8 *base = gUnk_020001AC;
    s32 i = 0;
    u8 *a = base + 0x1c;
    u8 *b = base + 0xf8;
    for (; i <= 11; i++) {
        if (*(u32 *)b == key)
            return a;
        a += 0x19c;
        b += 0x19c;
    }
    return 0;
}
