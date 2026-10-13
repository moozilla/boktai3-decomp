#include "global.h"
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectTiles(void *, u32);
void sub_08237DA0(u8 *s)
{
    u8 *p = s + 0x48;
    u8 *q;
    AnimData_Load(p, 0x210e);
    q = s + 0x1c;
    Sprite_Init(q, p, 1);
    *(u16 *)(q + 0x10) = 0;
    AnimData_SelectTiles(p, 0x4d);
}
