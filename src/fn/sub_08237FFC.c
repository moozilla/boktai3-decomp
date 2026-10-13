#include "global.h"

void Sprite_Remove(u8 *);

u32 sub_08237FFC(u8 *p)
{
    Sprite_Remove(p + 0x18);
    return 0;
}
