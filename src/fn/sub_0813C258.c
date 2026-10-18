#include "global.h"

void Sprite_Remove(u8 *);

void DirectionalSpriteEffect_RemoveSprite(u8 *p)
{
    Sprite_Remove(p + 0x7FC);
}
