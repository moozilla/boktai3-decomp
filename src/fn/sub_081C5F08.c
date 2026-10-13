#include "global.h"

void Anim_LoadKeyframe(u32, u32, u16, u32);

void sub_081C5F08(u32 a, u32 b, u32 c)
{
    if (c <= 9)
        Anim_LoadKeyframe(a, b, c, 0);
}
