#include "global.h"
void Collider_Remove(u8 *);
void Sprite_Remove(u8 *);
void sub_080464F0(s32 a, u8 *p)
{
    Collider_Remove(p + 0x6C);
    Sprite_Remove(p + 0xDC);
}
