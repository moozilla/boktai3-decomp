#include "global.h"

extern u16 gUnk_0300521C;
extern u8 gUnk_03005218;
extern u16 gUnk_03005234;

#define REG16(a) (*(vu16 *)(a))

void sub_08219134(void)
{
    if (gUnk_0300521C) {
        if (gUnk_03005218 & 1)
            REG16(0x04000008) |= 0x40;
        else
            REG16(0x04000008) &= ~0x40;
        if (gUnk_03005218 & 2)
            REG16(0x0400000A) |= 0x40;
        else
            REG16(0x0400000A) &= ~0x40;
        if (gUnk_03005218 & 4)
            REG16(0x0400000C) |= 0x40;
        else
            REG16(0x0400000C) &= ~0x40;
        if (gUnk_03005218 & 8)
            REG16(0x0400000E) |= 0x40;
        else
            REG16(0x0400000E) &= ~0x40;
        if (!(gUnk_03005218 & 0x10))
            REG16(0x0400004C) = *(u8 *)&gUnk_03005234;
        else
            REG16(0x0400004C) = gUnk_03005234;
        gUnk_0300521C = 0;
    }
}
