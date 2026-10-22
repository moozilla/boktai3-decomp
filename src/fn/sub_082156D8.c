#include "global.h"

extern void *gUnk_03004BC0[];
extern u16 gUnk_03004BE0[];
extern u16 gUnk_03004C08[];
extern u16 gUnk_03004BD8, gUnk_03004C14, gUnk_03004BFC, gUnk_03004BB0, gUnk_03004BEC;
extern s16 gUnk_03004BD0[4];
extern u32 gUnk_03004BF8, gUnk_03004C00;
void Bg_ClearAllTilemaps(void);

#define REG16(a) (*(vu16 *)(a))

void sub_082156D8(u32 mode)
{
    s32 i;

    Bg_ClearAllTilemaps();
    for (i = 0; i < 4; i++) {
        gUnk_03004BC0[i] = 0;
        gUnk_03004BE0[i] = 0;
        gUnk_03004C08[i] = 0;
    }
    gUnk_03004BD8 = 0;
    gUnk_03004C14 = 0x100;
    gUnk_03004BFC = 0x100;
    gUnk_03004BB0 = 0;
    gUnk_03004BD0[0] = 0x100;
    gUnk_03004BD0[1] = 0;
    gUnk_03004BD0[2] = 0;
    gUnk_03004BD0[3] = 0x100;
    gUnk_03004BF8 = 0;
    gUnk_03004C00 = 0;
    gUnk_03004BEC = mode;
    switch (mode) {
        case 0:
            REG16(0x04000008) = 0x3F08;
            REG16(0x0400000A) = 0x3E01;
            REG16(0x0400000C) = 0x3D03;
            REG16(0x0400000E) = 0x3C01;
            REG16(0x04000000) = 0x7160;
            break;
        case 1:
            REG16(0x04000008) = 0x3F08;
            REG16(0x0400000A) = 0x3E42;
            REG16(0x0400000C) = 0x9D81;
            REG16(0x04000000) = 0x7161;
            break;
    }
    {
        s32 mask = ~0xE00;
        gUnk_03004BD8 &= mask;
    }
}
