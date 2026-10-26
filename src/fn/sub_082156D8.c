#include "global.h"
#include "render.h"

void Bg_ClearAllTilemaps(void);

#define REG16(a) (*(vu16 *)(a))

void sub_082156D8(u32 mode)
{
    s32 i;

    Bg_ClearAllTilemaps();
    for (i = 0; i < 4; i++) {
        gBgTileUploadSrc[i] = 0;
        gBgTileUploadParamA[i] = 0;
        gBgTileUploadParamB[i] = 0;
    }
    gDispCntShadow = 0;
    gBgAffineScaleX = 0x100;
    gBgAffineScaleY = 0x100;
    gBgAffineAngle = 0;
    gBgAffineMatrix[0] = 0x100;
    gBgAffineMatrix[1] = 0;
    gBgAffineMatrix[2] = 0;
    gBgAffineMatrix[3] = 0x100;
    gBgAffineRefX = 0;
    gBgAffineRefY = 0;
    gBgAffineMode = mode;
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
        gDispCntShadow &= mask;
    }
}
