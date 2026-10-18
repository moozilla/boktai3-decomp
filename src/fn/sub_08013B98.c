#include "global.h"
void AnimData_Load(u8 *);
void Sprite_ApplyAnimation(u8 *, u8 *);
u32 ScaledShadow_SetGraphic(u8 *p, u32 b, u32 c)
{
    u8 *q = p + 0x38;
    AnimData_Load(q);
    p += 0xc;
    Sprite_ApplyAnimation(p, q);
    *(u16 *)(p + 0x10) = c;
}
