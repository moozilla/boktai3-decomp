#include "global.h"

extern u8 *gUnk_02000028;

u32 sub_08005E40(u8 *p) {
    u8 *prev = *(u8 **)(p + 0x1C);
    u8 *next = *(u8 **)(p + 0x20);
    if (prev) {
        *(u8 **)(prev + 0x20) = next;
    } else {
        *(u8 **)(gUnk_02000028 + 0x18) = next;
    }
    if (next) {
        *(u8 **)(next + 0x1C) = prev;
    }
    return 0;
}
