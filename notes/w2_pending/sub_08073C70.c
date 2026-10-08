#include "global.h"

extern u8 *gUnk_020004A8;

u32 sub_08073C70(u32 a, u32 b) {
    u8 *e = *(u8 **)(gUnk_020004A8 + 0x280);
    s32 i = 0;
    while (e) {
        if (e[4] == a && e[5] == b) return (u32)e;
        e = *(u8 **)e;
        i++;
        if (i > 11) break;
    }
    return 0;
}
