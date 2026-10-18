#include "global.h"

u32 Sprite_Remove(u8 *);

void ActorBurst_RemoveGlowSprite(u8 *p)
{
    Sprite_Remove(p + 0xC0);
}
