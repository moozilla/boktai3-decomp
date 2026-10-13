#include "global.h"

s32 Sprite_Remove(u8 *);

s32 sub_08202120(u8 *p)
{
    return Sprite_Remove(p + 0x24);
}
