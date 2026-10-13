#include "global.h"

void Anim_Remove(u8 *);

u32 sub_0819A59C(u8 *p)
{
    Anim_Remove(p + 0x18);
    return 0;
}
