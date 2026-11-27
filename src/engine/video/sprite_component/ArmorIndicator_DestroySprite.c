#include "global.h"

u32 Sprite_Remove(u8 *);

void ArmorIndicator_DestroySprite(u8 *p)
{
    Sprite_Remove(p + 0x8);
}
