#include "global.h"

extern u8 *gUnk_020001BC;
u32 sub_0821A520(u32, u32);

u32 sub_08108A1C(u8 *p) {
    *(u32 *)(p + 0x18) = sub_0821A520(0x922E, 0x9AF2);
    gUnk_020001BC = p;
    return *(u32 *)(p + 0x154) = 0;
}
