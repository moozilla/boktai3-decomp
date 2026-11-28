#include "effects/bursts/radial_burst.h"

s32 Div(s32, s32);
void SparkParticle_Spawn(void *, u32, const void *, u32, u32, u32, u32, u32);

void RadialBurstEffect_UpdateBurstParticle(struct RadialBurstParticle *particle)
{
    if (++particle->timer > 15) {
        particle->drawable.flags |= 1;
        particle->active = 0;
        SparkParticle_Spawn(&particle->drawable.position, 1, 0, 0, 0, 0, 2, 1);
    } else if (particle->timer > 11) {
        particle->drawable.position.x =
            particle->orbitOrigin->x +
            SpiralBurst_ScaleOffset(gUnk_086149C4[(particle->angle + 0x40) & 0xFF] * 10);
        particle->drawable.position.y = particle->orbitOrigin->y;
        particle->drawable.position.z =
            particle->orbitOrigin->z + SpiralBurst_ScaleOffset(gUnk_086149C4[particle->angle] * 10);
    } else {
        s32 remaining = 12 - particle->timer;
        s32 divisor = remaining + 1;

        particle->angle += particle->angularSpeed;
        particle->orbitRadius = Div(particle->orbitRadius * remaining, divisor);
        particle->drawable.position.x =
            particle->orbitOrigin->x +
            SpiralBurst_ScaleOffset(particle->orbitRadius *
                                    gUnk_086149C4[(particle->angle + 0x40) & 0xFF]);
        particle->drawable.position.y = particle->orbitOrigin->y;
        particle->drawable.position.z =
            particle->orbitOrigin->z +
            SpiralBurst_ScaleOffset(particle->orbitRadius * gUnk_086149C4[particle->angle]);
    }
}
