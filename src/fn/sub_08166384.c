#include "global.h"
void Anim_LoadKeyframe(s32, u8 *, u32, s32);
void PauseMenu_LoadIndexedIconKeyframe(u8 *p, s32 x, s32 c)
{
    Anim_LoadKeyframe(x, p + 0x64, (u16)(c + 0x90), 0);
}
