#include "global.h"
extern u16 gUnk_03004BD8;
void Anim_Remove(u8 *);
void sub_081DBDC8(u8 *p)
{
    u16 *r = &gUnk_03004BD8;
    u32 m = ~3;
    *r = m & *r;
    *(u32 *)(p + 0x4c) |= 1;
    Anim_Remove(p + 0x154);
    Anim_Remove(p + 0xcc);
    Anim_Remove(p + 0x44);
}
