#include "effects/bursts/radial_burst.h"

void RadialBurstEffect_UpdateParticle(struct RadialBurstParticle *particle,
                                      const struct SpriteEffectTileResource *graphics)
{
    if (++particle->timer > 13) {
        particle->drawable.flags |= 1;
        particle->active = 0;
    } else {
        particle->drawable.position.x += particle->vector.velocity.x;
        particle->drawable.position.y += particle->vector.velocity.y;
        particle->drawable.position.z += particle->vector.velocity.z;
        if (particle->timer == 10)
            SpriteDrawable_SelectTileFrame(&particle->drawable, graphics, particle->tile + 1);
    }
}
