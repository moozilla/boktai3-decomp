#include "global.h"

void sub_081D7874(u8 *p) {
    u8 z = 0;
    u32 one = 1;
    u8 *a = p + 0x70;
    u8 *b = p;
    s32 i = 3;
    do {
        *a = z;
        *(u32 *)(b + 0x48) |= one;
        a += 0x30;
        b += 0x30;
        i--;
    } while (i >= 0);
}
