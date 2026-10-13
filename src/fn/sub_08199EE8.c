#include "global.h"

void Collider_Remove(u8 *);
u32 Sprite_Remove(u8 *);

void sub_08199EE8(u8 *p)
{
    Collider_Remove(p + 0x60);
    Sprite_Remove(p + 0x24);
}
