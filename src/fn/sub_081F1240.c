#include "global.h"

void Sprite_Remove(u8 *);

void sub_081F1240(u8 *p)
{
    Sprite_Remove(p + 0x2C0);
    Sprite_Remove(p + 0x308);
}
