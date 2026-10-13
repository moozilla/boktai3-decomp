#include "global.h"
#define SpritePool_SetAnimationFrame sub_08019C64
extern u32 gUnk_020000B0;
void Anim_SetSequence(u8 *, u32, u32, u32, u32);
void SpritePool_SetAnimationFrame(u8 *p, u32 a, u32 b, u32 c)
{
    if (gUnk_020000B0)
        Anim_SetSequence(p + 0x54, *(u32 *)(p + 0x64), (u16)a, (u8)b, (u8)c);
}
