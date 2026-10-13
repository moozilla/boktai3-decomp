#include "global.h"
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void sub_081188C8(u8 *s, u32 x)
{
    Anim_SetKeyframes(s + 0x6d0, s + 0x7f0, (u16)x, 1, 0);
}
