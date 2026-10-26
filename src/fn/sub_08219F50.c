#include "global.h"
#include "engine.h"
struct Dma {
    vu32 src, dst, ctl;
};
extern const u32 gUnk_08614114[];
u32 TaskSystem_InitLists(void)
{
    struct Dma *d = (struct Dma *)0x040000D4;
    d->src = (u32)gUnk_08614114;
    d->dst = (u32)&gTaskLists;
    d->ctl = 0x8400001C;
    return d->ctl;
}
