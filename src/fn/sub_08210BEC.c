#include "global.h"
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
u32 sub_08210BEC(u8 *p)
{
    Anim_LoadKeyframe(p + 0x164, p + 0x24, (u16)(*(u16 *)(p + 0xb80) + 0x59), 0);
    return 0;
}
