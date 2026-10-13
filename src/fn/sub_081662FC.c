#include "global.h"
void Anim_LoadKeyframe(s32, u8 *, u32, s32);
void sub_081662FC(u8 *p, s32 x, s32 c)
{
    if (c <= 5)
        Anim_LoadKeyframe(x, p + 0xa4, (u16)(c + 0x184), 1);
    else
        Anim_LoadKeyframe(x, p + 0xa4, (u16)(c + 0x19c), 1);
}
