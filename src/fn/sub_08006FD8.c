#include "global.h"

extern u8 *gUnk_02000030;

u32 sub_08006FD8(u8 *p) {
    u8 *a = *(u8 **)(p + 0xB4);
    u8 *b = *(u8 **)(p + 0xB8);
    if (a) {
        *(u8 **)(a + 0xB8) = b;
    } else {
        *(u8 **)(gUnk_02000030 + 0x28) = b;
    }
    if (b) {
        *(u8 **)(b + 0xB4) = a;
    }
    return 0;
}
