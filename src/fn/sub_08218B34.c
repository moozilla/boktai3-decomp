#include "global.h"

struct Dma { vu32 src, dst, ctl; };
extern u32 gUnk_03001660;
extern u8 gUnk_03001560[];

void sub_08218B34(void)
{
    u32 i;
    u32 src = 0x02034C00;
    for (i = 0; i < gUnk_03001660; i++) {
        u32 dst = (gUnk_03001560[i] << 5) + 0x0600A000;
        struct Dma *d = (struct Dma *)0x040000D4;
        d->src = src;
        d->dst = dst;
        d->ctl = 0x84000008;
        d->ctl;
        src += 0x20;
    }
    gUnk_03001660 = 0;
}
