#include "effects/bursts/radial_burst.h"

void sub_08235E20(struct RadialBurstParticle *particle,
                  const struct SpriteEffectTileResource *graphics)
{
    if (++particle->timer > 11) {
        particle->drawable.flags |= 1;
        particle->active = 0;
        return;
    }
    if (particle->timer == 6)
        SpriteDrawable_SelectTileFrame(&particle->drawable, graphics, particle->tile + 1);
    if (particle->timer > 3) {
        particle->speed++;
        if (particle->timer > 8)
            particle->angularSpeed--;
    }
    particle->angle += particle->angularSpeed;
    particle->drawable.position.x =
        particle->vector.origin.x +
        SpiralBurst_ScaleOffset(particle->radius * gUnk_086149C4[(particle->angle + 64) & 255]);
    particle->drawable.position.z =
        particle->vector.origin.z +
        SpiralBurst_ScaleOffset(particle->radius * gUnk_086149C4[particle->angle]);
    particle->drawable.position.y += particle->speed;
}
