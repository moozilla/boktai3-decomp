#include "global.h"
extern u32 gUnk_020002F8[];
void Anim_Remove(u8 *);
void sub_081DC448(u8 *p)
{
    if (gUnk_020002F8[2] == 1) {
        Anim_Remove(p + 0xa4);
        Anim_Remove(p + 0x1c);
    }
}
