#include "global.h"
#include "ram.h"
/* SpritePool_SetAnimationFrame: Recovered byte-tail function; advances the slot's running animation sequence using its stored script handle when the pool is active */
#define SpritePool_SetAnimationFrame sub_08019C64
#define SpritePool_SetAnimationFrame sub_08019C64
void Anim_SetSequence(u8 *, u32, u32, u32, u32);
void SpritePool_SetAnimationFrame(u8 *p, u32 a, u32 b, u32 c)
{
    if (gSpritePoolManager)
        Anim_SetSequence(p + 0x54, *(u32 *)(p + 0x64), (u16)a, (u8)b, (u8)c);
}
