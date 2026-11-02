// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"
extern const s16 gUnk_086149C4[];
void SceneMovingParticle_Create(struct ContactPosition *, struct ContactPosition *, s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *input)
{
    struct ContactPosition *position = input;
    u32 savedOrigin;
    u32 origin;
    const u16 *table;
    struct ContactPosition velocity;
    struct ContactPosition *v;
    s32 angle;
    s32 magnitude;
    register u32 *cursor asm("r3");
    u32 mask;
    origin = PARTICLE_RANDOM_SAMPLE_ORIGIN;
    asm volatile("" : "+r"(origin));
    table = gRandomTable;
    {
        register u32 index asm("r0") = ((cursor = &gRandomIndex), *cursor + 1) & 1023;
        const u8 *sample = (void *)(index * 2 + (u32)table);
        mask = 255;
        angle = *sample;
        index = (index + 1) & 1023;
        *cursor = index;
        {
            s32 sample = table[index];
            s32 mask = 31;
            sample &= mask;
            v = &velocity;
            magnitude = sample - 16;
        }
    }
    {
        register const s16 *sine asm("r1") = gUnk_086149C4;
        s32 product = sine[(angle + 64) & mask] * magnitude;
        PARTICLE_RANDOM_SAVE_ORIGIN(savedOrigin, origin);
        v->x = Truncate(product);
        v->y = 0;
        {
            s32 product = sine[angle] * magnitude;
            v->z = Truncate(product);
        }
    }
    {
        u32 offset;
        const u16 *source;
        *cursor = (*cursor + 1) & 1023;
        offset = savedOrigin + *cursor * 2;
        source = (const u16 *)RANDOM_TABLE_ADDR;
        {
            u32 value = ParticleRandom_ReadSampleAddress(offset, savedOrigin, source);
            {
                u32 mask = 63;
                value &= mask;
            }
            value -= 48;
            velocity.y = value;
        }
    }
    SceneMovingParticle_Create(position, &velocity, 60, 3);
}
