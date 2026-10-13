#include "global.h"

u32 Sprite_Remove(u8 *);

void sub_08236FE8(u8 *p)
{
    Sprite_Remove(p + 0x2C);
}
