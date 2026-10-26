#include "global.h"
#include "render.h"

#define REG16(a) (*(vu16 *)(a))

void sub_08219134(void)
{
    if (gMosaicHalfword) {
        if (gMosaicByte & 1)
            REG16(0x04000008) |= 0x40;
        else
            REG16(0x04000008) &= ~0x40;
        if (gMosaicByte & 2)
            REG16(0x0400000A) |= 0x40;
        else
            REG16(0x0400000A) &= ~0x40;
        if (gMosaicByte & 4)
            REG16(0x0400000C) |= 0x40;
        else
            REG16(0x0400000C) &= ~0x40;
        if (gMosaicByte & 8)
            REG16(0x0400000E) |= 0x40;
        else
            REG16(0x0400000E) &= ~0x40;
        if (!(gMosaicByte & 0x10))
            REG16(0x0400004C) = *(u8 *)&gMosaicParam;
        else
            REG16(0x0400004C) = gMosaicParam;
        gMosaicHalfword = 0;
    }
}
