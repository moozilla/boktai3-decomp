#include "player/effects/spiral_trail.h"

void Sprite_Remove(void *);

void SpiralTrail_RemoveSprite(struct SpiralTrailTask *p)
{
    Sprite_Remove(&p->sprite.drawable);
}
