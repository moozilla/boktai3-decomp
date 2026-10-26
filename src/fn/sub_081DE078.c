#include "global.h"
#include "render.h"
void Anim_Remove(void *);
void NameEntry_Destroy(u8 *p)
{
    u16 *q = &gDispCntShadow;
    u32 m = ~3;
    *q = m & *q;
    Anim_Remove(p + 0x2c0);
    Anim_Remove(p + 0x238);
    Anim_Remove(p + 0x1b0);
    Anim_Remove(p + 0x128);
    Anim_Remove(p + 0xa0);
    Anim_Remove(p + 0x18);
}
