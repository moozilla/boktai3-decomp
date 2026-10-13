#include "global.h"
void Sprite_Remove(u8 *);
u32 sub_0805400C(u8 *p)
{
    Sprite_Remove(p + 0x1C);
    Sprite_Remove(p + 0x6C);
    return 0;
}
