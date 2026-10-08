#include "global.h"

struct Dma { vu32 src, dst, ctl; };
extern u32 gUnk_030051E0;
extern s16 gUnk_03004DB0;
extern u16 gUnk_030051D0;

void sub_082173A8(void)
{
    struct Dma *d = (struct Dma *)0x040000D4;
    d->src = gUnk_030051E0;
    d->dst = 0x05000000;
    d->ctl = 0x84000080;
    d->ctl;
    d->src = 0x03004DC0;
    d->dst = 0x05000200;
    d->ctl = 0x84000080;
    d->ctl;
    if (gUnk_03004DB0 == 0) {
        *(u16 *)0x05000000 = 0x1084;
    } else {
        *(u16 *)0x05000000 = gUnk_030051D0;
        gUnk_030051D0 = 0x1084;
        gUnk_03004DB0 = 0;
    }
}
