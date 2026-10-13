#include "global.h"

void Sprite_Remove(u8 *);

u32 sub_081F2BFC(u8 *p)
{
    Sprite_Remove(p + 0x18);
    return 0;
}
