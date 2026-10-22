#include "global.h"

struct Dma {
    vu32 src, dst, ctl;
};
extern u16 gUnk_03004BD8;
extern void *gUnk_03004BC0[];
extern u16 gUnk_03004C08[];
extern u16 gUnk_03004BE0[];
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

    if (gUnk_03004BD8 & 0x100)
        CopyMap(0, 0x0600F800);
    if (gUnk_03004BD8 & 0x200)
        CopyMap(1, 0x0600F000);
    if (gUnk_03004BD8 & 0x400)
        CopyMap(2, 0x0600E800);
    if (gUnk_03004BD8 & 0x800)
        CopyMap(3, 0x0600E000);
    for (i = 0; i <= 3; i++) {
        if (gUnk_03004BC0[i]) {
            BgGfx_LoadTiles(gUnk_03004BC0[i], (u8 *)0x06000000 + gUnk_03004C08[i],
                            gUnk_03004BE0[i] << 5);
            gUnk_03004BC0[i] = 0;
        }
    }
}
