#include "global.h"
void Sprite_Remove(u8 *);
void Anim_Remove(u8 *);
void sub_0803A350(s32 a, u8 *p)
{
    Sprite_Remove(p + 0x24);
    Anim_Remove(p + 0x50);
}
