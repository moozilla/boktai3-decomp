#include "effects/healing/healing_effect.h"

void FlickerParticle_UpdateStationary(struct HealingParticle *particle)
{
    if (++particle->age > 7) {
        particle->drawable.flags |= 1;
        particle->active = 0;
    } else {
        SpriteDrawable_SelectTileFrame(&particle->drawable, particle->resource,
                                       2 + ((particle->age >> 2) & 1));
    }
}
