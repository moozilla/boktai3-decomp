#include "global.h"
#include "render.h"
extern const u32 gUnk_086140CC[];
void Bg_ClearTilemap(s32);
struct Dma {
    vu32 src, dst, ctl;
};
void sub_082157EC(void)
{
    u32 zero;
    s32 i;
    gDispCntShadow = 0;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    *(vu16 *)0x04000008 &= 0xffbf;
    *(vu16 *)0x0400000a &= 0xffbf;
    *(vu16 *)0x0400000c &= 0xffbf;
    *(vu16 *)0x0400000e &= 0xffbf;
    *(vu16 *)0x04000050 = 0;
    *(vu16 *)0x04000052 = 0;
    *(vu16 *)0x04000054 = 0;
    i = 0;
    zero = 0;
    for (; i <= 3; i++) {
        struct Dma *dma = (struct Dma *)0x040000d4;
        gBgTileUploadSrc[i] = zero;
        gBgTileUploadParamA[i] = zero;
        gBgTileUploadParamB[i] = zero;
        dma->src = (u32)gUnk_086140CC;
        dma->dst = 0x06000000 + i * 0x4000;
        dma->ctl = 0x84000008;
        dma->ctl;
    }
}
