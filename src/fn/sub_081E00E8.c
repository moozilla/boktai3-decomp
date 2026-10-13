#include "global.h"
extern u16 gUnk_03004BD8;
void Anim_Remove(void *);
void sub_081E00E8(u8 *p)
{
    u16 *q = &gUnk_03004BD8;
    u32 m = ~3;
    *q = m & *q;
    Anim_Remove(p + 0x2c0);
    Anim_Remove(p + 0x238);
    Anim_Remove(p + 0x128);
    Anim_Remove(p + 0x1b0);
    Anim_Remove(p + 0xa0);
    Anim_Remove(p + 0x18);
}
