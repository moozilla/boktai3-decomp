#include "global.h"

s32 Sprite_Remove(u8 *);

s32 sub_08162A30(u8 *p)
{
    Sprite_Remove(p + 0xf4);
    Sprite_Remove(p + 0x120);
    return 0;
}
