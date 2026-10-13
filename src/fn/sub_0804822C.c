#include "global.h"

void Collider_Remove(u8 *);
void Sprite_Remove(u8 *);
u32 sub_0804822C(u8 *p)
{
    Collider_Remove(p + 0x70);
    Sprite_Remove(p + 0x44);
    return 0;
}
