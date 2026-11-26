#include "global.h"

u32 Sprite_Remove(u8 *);

void Nidhoggr_DestroySpriteEffect(u8 *p)
{
    Sprite_Remove(p + 0x24);
}
