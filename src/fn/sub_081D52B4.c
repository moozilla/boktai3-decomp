#include "global.h"

extern u8 *gUnk_02000710;

u32 sub_081D52B4(u32 a, u32 b) {
    u32 *p = (u32 *)(gUnk_02000710 + 0x700);
    u32 idx = (a << 1) + b;
    u32 w = idx >> 5;
    u32 bit;
    if (w > 7) {
        return 0;
    }
    p += w;
    bit = 1 << (idx & 0x1F);
    if (*p & bit) {
        return 1;
    }
    return 0;
}
