#include "global.h"
#include "render.h"

struct Dma {
    vu32 src, dst, ctl;
};

void Text_FlushTileUploads(void)
{
    u32 i;
    u32 src = 0x02034C00;
    for (i = 0; i < gTextTileUploadCount; i++) {
        u32 dst = (gTextTileUploadIndices[i] << 5) + 0x0600A000;
        struct Dma *d = (struct Dma *)0x040000D4;
        d->src = src;
        d->dst = dst;
        d->ctl = 0x84000008;
        d->ctl;
        src += 0x20;
    }
    gTextTileUploadCount = 0;
}
