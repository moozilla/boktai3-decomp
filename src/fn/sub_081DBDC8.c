#include "global.h"
#include "render.h"
void Anim_Remove(u8 *);
void ScreenParams2E8_DestroyScreen(u8 *p)
{
    u16 *r = &gDispCntShadow;
    u32 m = ~3;
    *r = m & *r;
    *(u32 *)(p + 0x4c) |= 1;
    Anim_Remove(p + 0x154);
    Anim_Remove(p + 0xcc);
    Anim_Remove(p + 0x44);
}
