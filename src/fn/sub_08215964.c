#include "global.h"
#include "render.h"

struct Dma {
    vu32 src, dst, ctl;
};
void *Video_GetBackgroundMap(u32);
void BgGfx_LoadTiles(u8 *source, u8 *destination, u32 bytes);

#define CopyMap(layer, dest)                                                                       \
    {                                                                                              \
        struct Dma *d = (struct Dma *)0x040000D4;                                                  \
        d->src = (u32)Video_GetBackgroundMap(layer);                                               \
        d->dst = (dest);                                                                           \
        d->ctl = 0x84000200;                                                                       \
        d->ctl;                                                                                    \
    }

void sub_08215964(void)
{
    s32 i;

    if (gDispCntShadow & 0x100)
        CopyMap(0, 0x0600F800);
    if (gDispCntShadow & 0x200)
        CopyMap(1, 0x0600F000);
    if (gDispCntShadow & 0x400)
        CopyMap(2, 0x0600E800);
    if (gDispCntShadow & 0x800)
        CopyMap(3, 0x0600E000);
    for (i = 0; i <= 3; i++) {
        if (gBgTileUploadSrc[i]) {
            BgGfx_LoadTiles(gBgTileUploadSrc[i], (u8 *)0x06000000 + gBgTileUploadParamB[i],
                            gBgTileUploadParamA[i] << 5);
            gBgTileUploadSrc[i] = 0;
        }
    }
}
