#include "global.h"
void Anim_LoadKeyframe(s32, u8 *, u32, s32);
void sub_08166354(u8 *p, s32 x)
{
    Anim_LoadKeyframe(x, p + 0xa4, 0xc9, 1);
}
