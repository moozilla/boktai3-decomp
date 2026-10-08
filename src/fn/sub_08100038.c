#include "global.h"

extern u8 *gUnk_020001A4;
u32 sub_0821A520(u32, u32);

u32 sub_08100038(u8 *p) {
    *(u32 *)(p + 0x18) = sub_0821A520(0x922E, 0xB952);
    gUnk_020001A4 = p;
    return *(u32 *)(p + 0xA3C) = 0;
}
