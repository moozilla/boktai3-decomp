#include "scene_actor_control.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"

void SceneActorControl_TickJitter(void *unused, struct SceneActorControlSlot *slot)
{
    if (slot->timer <= 0) {
        slot->timer = 20;
        slot->flags = (s16)slot->flags & ~4;
    } else {
        if (slot->timer > 10) {
            union ParticleRandomTableSource source;
            const u16 *table;
            u32 *seed;
            u32 mask;
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
                slot->position.x += Mod(*sample, 40) - 20;
            }
            slot->position.z += Mod(ParticleRandom_NextSample(table, seed, mask), 40) - 20;
        }
        slot->timer--;
    }
}
