#include "global.h"
void Anim_SetKeyframes(u8 *, u8 *, u16, u32, u32);
void sub_08120304(u8 *a, u32 b)
{
    Anim_SetKeyframes(a + 0x6b8, a + 0x7d8, b, 1, 0);
}
