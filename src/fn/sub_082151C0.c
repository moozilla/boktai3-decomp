#include "global.h"

struct Hdr { u8 pad[0xC]; u32 fC; u32 f10; u32 f14; };
extern struct Hdr *gUnk_03004300;
extern u32 gUnk_030042FC;
extern u32 gUnk_030042F8;

void sub_082151C0(struct Hdr *h)
{
    gUnk_03004300 = h;
    gUnk_030042FC = (u32)h + h->f14;
    gUnk_030042F8 = (u32)h + h->fC;
}
