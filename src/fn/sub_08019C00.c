#include "global.h"
extern u32 gUnk_020000B0;
void sub_08019C00(u8 *p, u32 *src) {
    u32 first;
    u32 second;
    if (gUnk_020000B0 != 0) {
        first = src[0];
        second = src[1];
        *(u32 *)(p + 0x28) = first;
        *(u32 *)(p + 0x2C) = second;
    }
}
