#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"

#include "npc/otenko/scene_npc_transition.h"

s32 Mod(s32, s32);

/* Negative angles scatter a particle around the origin; other angles start at the origin. */
void Otenko_SpawnTransitionParticle(struct TransitionEffect *effect, s32 angle)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        struct TransitionParticle *particle = &effect->particles[i];
        if (particle->motion.active == 0) {
            union ParticleRandomTableSource source;
            const u16 *table;
            u32 *seed;
            particle->motion.active = 1;
            particle->motion.angle = angle;
            particle->motion.age = 0;
            source.halves[0] = (u16)RANDOM_TABLE_ADDR;
            source.halves[1] = RANDOM_TABLE_ADDR >> 16;
            table = ParticleRandom_GetTable(&source);
            seed = &gRandomIndex;
            {
                u32 index = *seed + 1;
                const u16 *sample;
                index &= 0x3FF;
                *seed = index;
                sample = ParticleRandom_SampleAddress(table, *seed);
                particle->motion.lifetime = Mod(*sample, 10) + 10;
            }
            if (angle < 0) {
                {
                    s32 random = Mod(ParticleRandom_NextSample(table, seed, 0x3FF), 160);
                    u32 value = (u16)effect->origin.x;
                    value -= 80;
                    particle->motion.position.x = value + random;
                }
                {
                    s32 random = Mod(ParticleRandom_NextSample(table, seed, 0x3FF), 80);
                    u32 value = (u16)effect->origin.y;
                    value -= 40;
                    particle->motion.position.y = value + random;
                }
                {
                    s32 random = Mod(ParticleRandom_NextSample(table, seed, 0x3FF), 160);
                    u32 value = (u16)effect->origin.z;
                    value -= 80;
                    particle->motion.position.z = value + random;
                }
                particle->motion.verticalVelocity = 0;
            } else {
                particle->motion.position.x = effect->origin.x;
                particle->motion.position.y = effect->origin.y;
                particle->motion.position.z = effect->origin.z;
                particle->motion.verticalVelocity = 15;
            }
            particle->flags &= ~1;
            return;
        }
    }
}
