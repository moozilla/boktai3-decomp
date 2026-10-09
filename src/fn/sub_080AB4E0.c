#include "global.h"
void sub_080AB4E0(u8 *base) {
    u8 *ctx = *(u8 **)(base + 0x3D0);
    u16 *src = (u16 *)(base + 0x2F4);
    u16 *dst = (u16 *)(ctx + 0x87C);
    s32 i = 5;
    do {
        *dst++ = *src;
        i--;
    } while (i >= 0);
    ctx[0x878] = 0;
}
