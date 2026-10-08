#include "global.h"
struct Dma { vu32 src, dst, ctl; };
extern const u32 gUnk_08614114[];
extern u32 gUnk_03005280;
u32 sub_08219F50(void)
{
    struct Dma *d = (struct Dma *)0x040000D4;
    d->src = (u32)gUnk_08614114;
    d->dst = (u32)&gUnk_03005280;
    d->ctl = 0x8400001C;
    return d->ctl;
}
