#include "global.h"
void Collider_Remove(u8 *);
void Sprite_Remove(u8 *);
u32 sub_08012F0C(u32 a, u8 *p)
{
    Collider_Remove(p + 0x48);
    Sprite_Remove(p);
    return 0;
}
