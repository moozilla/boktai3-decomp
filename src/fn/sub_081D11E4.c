#include "global.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"

#include "scene_npc_transition.h"

s32 Mod(s32, s32);

/* Negative angles scatter a particle around the origin; other angles start at the origin. */
void SceneNpc_SpawnTransitionParticle(struct TransitionEffect *effect, s32 angle)
{
    s32 i = 0;
    struct TransitionParticle *particle = effect->particles;
    struct TransitionMotion *state = &particle->motion;
    do {
        if (state->active == 0) {
            union ParticleRandomTableSource source;
            const u16 *table;
            u32 *seed;
            u32 mask;
            state->active = 1;
            state->angle = angle;
            state->age = 0;
            source.halves[0] = (u16)RANDOM_TABLE_ADDR;
            source.halves[1] = RANDOM_TABLE_ADDR >> 16;
            table = ParticleRandom_GetTable(&source);
            seed = &gRandomIndex;
            {
                u32 index = *seed + 1;
                const u16 *sample;
                mask = 0x3FF;
                index &= mask;
                *seed = index;
                sample = ParticleRandom_SampleAddress(table, *seed);
                state->lifetime = Mod(*sample, 10) + 10;
            }
            if (angle < 0) {
                {
                    s32 random = Mod(ParticleRandom_NextSample(table, seed, mask), 160);
                    u32 value = (u16)effect->origin.x;
                    value -= 80;
                    state->position.x = value + random;
                }
                {
                    s32 random = Mod(ParticleRandom_NextSample(table, seed, mask), 80);
                    u32 value = (u16)effect->origin.y;
                    value -= 40;
                    state->position.y = value + random;
                }
                {
                    s32 random = Mod(ParticleRandom_NextSample(table, seed, mask), 160);
                    u32 value = (u16)effect->origin.z;
                    value -= 80;
                    state->position.z = value + random;
                }
                state->verticalVelocity = 0;
            } else {
                state->position.x = effect->origin.x;
                state->position.y = effect->origin.y;
                state->position.z = effect->origin.z;
                state->verticalVelocity = 15;
            }
            particle->flags &= ~1;
            return;
        }
        particle++;
        state = (struct TransitionMotion *)((u8 *)state + sizeof(*particle));
        i++;
    } while (i < 8);
}
