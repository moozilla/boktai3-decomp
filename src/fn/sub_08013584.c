#include "global.h"

extern u8 *gUnk_02000054;
u32 sub_08215184(u32 a);

u32 sub_08013584(u8 *p) {
    gUnk_02000054 = p;
    *(u32 *)(p + 0x1C) = 0;
    *(u32 *)(p + 0x18) = sub_08215184(0x1C1A);
    return 0;
}
