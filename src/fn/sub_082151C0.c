#include "global.h"
#include "render.h"

struct Hdr {
    u8 pad[0xC];
    u32 fC;
    u32 f10;
    u32 f14;
};
extern struct Hdr *gUnk_03004300;

void AnimData_SetResourceBank(struct Hdr *h)
{
    gUnk_03004300 = h;
    gAnimResourceSection14 = (u32)h + h->f14;
    gAnimResourceSection0C = (u32)h + h->fC;
}
