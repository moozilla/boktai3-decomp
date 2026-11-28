#include "effects/bursts/radial_burst.h"
#include "engine/math/monster_particle_random.h"

void sub_08235E20(struct RadialBurstParticle *, const struct SpriteEffectTileResource *);

void CallbackGroup_EmitAtPosition(struct RadialBurst *burst, struct ContactPosition *origin)
{
    struct RadialBurstEmitter *emitter = &burst->emitter;
    struct RadialBurstParticle *particle;
    struct SpriteDrawable *drawable;

    if (++emitter->emitDelay > 1) {
        particle = &emitter->particles[emitter->nextParticle];
        particle->active = 1;
        particle->speed = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) + 0x10;
        particle->radius = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) + 0x40;
        particle->angle = ParticleRandom_ReadDirectSample(gRandomTable);
        particle->angularSpeed = 8;
        particle->timer = 0;
        particle->vector.origin = *origin;
        particle->update = sub_08235E20;
        drawable = &particle->drawable;
        drawable->flags &= ~1;
        SpriteDrawable_SelectTileFrame(drawable, burst->emitter.graphics, particle->tile);
        drawable->position.x =
            particle->vector.origin.x +
            SpiralBurst_ScaleOffset(particle->radius *
                                    gUnk_086149C4[(particle->angle + 0x40) & 0xFF]);
        drawable->position.y = particle->vector.origin.y;
        drawable->position.z =
            particle->vector.origin.z +
            SpiralBurst_ScaleOffset(particle->radius * gUnk_086149C4[particle->angle]);
        drawable->priority = 2;
        if (++emitter->nextParticle > 11)
            emitter->nextParticle = 0;
        emitter->emitDelay = 0;
        burst->emitter.particlesActive = 1;
    }
}
