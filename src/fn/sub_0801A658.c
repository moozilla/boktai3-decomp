#include "global.h"
void Anim_Remove(u8 *);
void Sprite_Remove(u8 *);
void sub_0801A658(u8 *p)
{
    Anim_Remove(p + 0xd4);
    Anim_Remove(p + 0x134);
    Sprite_Remove(p + 0x8c);
}
