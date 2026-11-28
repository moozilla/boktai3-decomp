#include "effects/healing/healing_effect.h"

void FlickerParticle_UpdateSecondaryMotion(struct HealingParticle *particle)
{
    if (++particle->age > 15) {
        particle->drawable.flags |= 1;
        particle->active = 0;
    } else {
        SpriteDrawable_SelectTileFrame(&particle->drawable, particle->resource,
                                       2 + ((particle->age >> 2) & 1));
        particle->drawable.position.x += particle->velocity.x;
        particle->drawable.position.y += particle->velocity.y;
        if (!(particle->age & 7))
            particle->velocity.y++;
    }
}
