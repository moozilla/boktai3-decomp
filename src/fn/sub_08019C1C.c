#include "global.h"
#include "ram.h"
u32 ScriptProc_Start(u32, u32);
void Anim_SetSequence(u8 *, u32, u32, u32, u32);
void SpritePool_StartAnimation(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    if (gSpritePoolManager != 0) {
        u32 r = ScriptProc_Start(0x922e, (u16)a);
        *(u32 *)(p + 0x64) = r;
        Anim_SetSequence(p + 0x54, r, (u16)b, (u8)c, (u8)d);
    }
}
