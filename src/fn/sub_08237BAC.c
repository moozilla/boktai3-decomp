#include "global.h"

void sub_08237BAC(u8 *p) {
    u8 z = 0;
    u32 one = 1;
    u8 *a = p + 0xA4;
    u8 *b = p;
    s32 i = 3;
    do {
        *a = z;
        *(u32 *)(b + 0x7C) |= one;
        a += 0x30;
        b += 0x30;
        i--;
    } while (i >= 0);
}
