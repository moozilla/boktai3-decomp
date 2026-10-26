#include "global.h"
#include "render.h"

struct Dma {
    vu32 src, dst, ctl;
};

void Palette_UploadBuffers(void)
{
    struct Dma *d = (struct Dma *)0x040000D4;
    d->src = gBgPaletteSource;
    d->dst = 0x05000000;
    d->ctl = 0x84000080;
    d->ctl;
    d->src = 0x03004DC0;
    d->dst = 0x05000200;
    d->ctl = 0x84000080;
    d->ctl;
    if (gBackdropOverride == 0) {
        *(u16 *)0x05000000 = 0x1084;
    } else {
        *(u16 *)0x05000000 = gBackdropColor;
        gBackdropColor = 0x1084;
        gBackdropOverride = 0;
    }
}
