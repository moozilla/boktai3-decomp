#include "global.h"

extern u8 *gUnk_020001B0;
u32 sub_0821A520(u32, u32);

u32 sub_08104E80(u8 *p) {
    *(u32 *)(p + 0x18) = sub_0821A520(0x922E, 0x74C9);
    gUnk_020001B0 = p;
    return *(u32 *)(p + 0x62C) = 0;
}
