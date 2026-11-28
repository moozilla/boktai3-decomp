#include "effects/bursts/radial_burst.h"
#include "engine/math/monster_particle_random.h"

void RadialBurstEffect_UpdateBurstParticle(struct RadialBurstParticle *);

void RadialBurstEffect_EmitOrbitParticle(struct RadialBurst *burst, struct ContactPosition *origin)
{
    struct RadialBurstEmitter *emitter = &burst->emitter;
    struct RadialBurstParticle *particle;

    if (++emitter->emitDelay > 2) {
        particle = &emitter->particles[emitter->nextParticle];
        particle->active = 1;
        particle->orbitOrigin = origin;
        particle->orbitRadius = (ParticleRandom_ReadDirectSample(gRandomTable) & 0x7F) + 0x180;
        particle->angle = ParticleRandom_ReadDirectSample(gRandomTable);
        particle->angularSpeed = (ParticleRandom_ReadDirectSample(gRandomTable) & 1) + 2;
        particle->timer = 0;
        particle->update = RadialBurstEffect_UpdateBurstParticle;
        particle->drawable.flags &= ~1;
        SpriteDrawable_SelectTileFrame(&particle->drawable, burst->emitter.graphics, 4);
        particle->drawable.position.x =
            particle->orbitOrigin->x +
            SpiralBurst_ScaleOffset(particle->orbitRadius *
                                    gUnk_086149C4[(particle->angle + 0x40) & 0xFF]);
        particle->drawable.position.y = particle->orbitOrigin->y;
        particle->drawable.position.z =
            particle->orbitOrigin->z +
            SpiralBurst_ScaleOffset(particle->orbitRadius * gUnk_086149C4[particle->angle]);
        particle->drawable.priority = 1;
        if (++emitter->nextParticle > 11)
            emitter->nextParticle = 0;
        emitter->emitDelay = 0;
        burst->emitter.particlesActive = 1;
    }
}
