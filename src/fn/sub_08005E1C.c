#include "global.h"

extern u8 *gUnk_02000028;

u32 sub_08005E1C(u8 *p) {
    u8 *t;
    *(u32 *)(p + 0x1C) = 0;
    t = *(u8 **)(gUnk_02000028 + 0x18);
    *(u8 **)(p + 0x20) = t;
    if (t) {
        *(u8 **)(t + 0x1C) = p;
    }
    *(u8 **)(gUnk_02000028 + 0x18) = p;
    return 0;
}
