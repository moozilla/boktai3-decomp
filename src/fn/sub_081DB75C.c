#include "global.h"
void Anim_Remove(u8 *);
void sub_081DB75C(u8 *p)
{
    Anim_Remove(p + 0x244);
    Anim_Remove(p + 0x1bc);
    Anim_Remove(p + 0x134);
    Anim_Remove(p + 0xac);
    Anim_Remove(p + 0x24);
}
