#include "global.h"

void Sprite_Remove(u8 *);

void sub_08238F10(u8 *p)
{
    Sprite_Remove(p + 0x7FC);
}
