#include "global.h"

void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);

void sub_08040ABC(u8 *p)
{
    u8 *q = p + 0x2fc;
    Anim_LoadKeyframe(q, p + 0x15c, 1, 0);
    *(u32 *)(q + 8) &= ~1;
}
