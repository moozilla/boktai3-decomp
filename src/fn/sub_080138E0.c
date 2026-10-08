#include "global.h"

extern u8 *gUnk_02000058;

u32 sub_080138E0(u8 *p) {
    gUnk_02000058 = p;
    *(u32 *)(p + 0x1C) = 0;
    return 0;
}
