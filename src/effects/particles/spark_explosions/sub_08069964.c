#include "effects/particles/burst_emitter.h"
#include "engine/math/monster_particle_random.h"
void BurstSpark_Create(const struct ContactPosition *, s32, s32, s32, s32, s32, u32);
void TrapSpellParticles_CreateBurst(const struct ContactPosition *, s32, s32, s32, s32, s32, s32,
                                      s32, s32, s32, s32, s32, s32);
void SparkExplosion_EmitFrame(struct BurstEmitter *emitter)
{
    s32 index;
    struct ContactPosition *origin;
    switch (emitter->frame) {
        case 0:
            if (!emitter->context)
                Sound_PlaySong(0x165);
            for (index = 0; index <= 5; index++)
                BurstSpark_Create(&emitter->position, 1, 0, index << 6, 84, 1, emitter->context);
            break;
        case 1: {
            struct ContactPosition *position;
            index = 0;
            position = &emitter->position;
            for (; index <= 3; index++) {
                BurstSpark_Create(position, 1, 8, index << 6, 34, 1, emitter->context);
                BurstSpark_Create(position, 1, 14, index << 6, 64, 1, emitter->context);
            }
            break;
        }
        case 2: {
            const u16 *table;
            index = 0;
            origin = &emitter->position;
            table = gRandomTable;
            for (; index <= 5; index++) {
                s32 value = ParticleRandom_ReadDirectSample(table) & 15;
                BurstSpark_Create(origin, 1, value, index << 6, 150, 1, emitter->context);
            }
            break;
        }
        case 3: {
            const u16 *table;
            index = 0;
            origin = &emitter->position;
            table = gRandomTable;
            for (; index <= 7; index++) {
                s32 value = ParticleRandom_ReadDirectSample(table) & 31;
                BurstSpark_Create(origin, 1, value, index << 5, 100, 1, emitter->context);
            }
            break;
        }
        case 4: {
            u32 *current;
            u32 mask;
            const u16 *table;
            index = 0;
            origin = &emitter->position;
            current = &gRandomIndex;
            mask = 0x3FF;
            table = gRandomTable;
            for (; index <= 15; index++) {
                u32 random = *current;
                s32 value;
                s32 duration;
                random = (random + 1) & mask;
                value = *(u16 *)((random << 1) + (u32)table);
                value &= 15;
                value += 5;
                random = (random + 1) & mask;
                *current = random;
                random <<= 1;
                random += (u32)table;
                duration = *(u16 *)random;
                duration &= 63;
                duration += 80;
                BurstSpark_Create(origin, 1, value, index << 4, duration, 2, emitter->context);
            }
            break;
        }
        case 5:
            if (!emitter->context) {
                u32 kind = 2;
                for (index = 0; index <= 7; index++)
                    TrapSpellParticles_CreateBurst(&emitter->position, 8, 16, 4, 12, 18,
                                                     index << 5, 8, 64, 1, kind, 512, 8);
            }
            break;
    }
}
