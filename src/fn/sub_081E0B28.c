#include "global.h"
void Anim_Remove(u8 *);
void sub_081E0B28(u8 *p)
{
    Anim_Remove(p + 0x12c);
    Anim_Remove(p + 0xa4);
    Anim_Remove(p + 0x1c);
}
