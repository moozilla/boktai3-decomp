#include "global.h"

void Anim_LoadKeyframe(u8 *, u8 *, u16, s32);

void sub_081EAA80(u8 *p)
{
    u8 *a = p + 0x128;
    u8 *b = p + 0x4E0;
    u16 v = (u16)(*(u32 *)(p + 0x724) + 0xC);
    Anim_LoadKeyframe(a, b, v, 0);
}
