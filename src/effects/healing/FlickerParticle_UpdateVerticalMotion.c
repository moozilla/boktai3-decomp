#include "effects/healing/healing_effect.h"

void FlickerParticle_UpdateVerticalMotion(struct HealingParticle *particle)
{
    if (++particle->age > 15) {
        particle->drawable.flags |= 1;
        particle->active = 0;
    } else {
        SpriteDrawable_SelectTileFrame(&particle->drawable, particle->resource,
                                       2 + ((particle->age >> 2) & 1));
        particle->drawable.position.y += particle->velocity.y;
    }
}
