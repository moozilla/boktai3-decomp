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
void SceneSteeringActor_EmitRandomParticle(struct ContactPosition *input)
{
    struct ContactPosition *position = input;
    u32 origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
    u32 savedOrigin;
    const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
    struct ContactPosition velocity;
    struct ContactPosition *v;
    s32 angle;
    s32 magnitude;
    s32 speed;
    u32 mask;

    {
        u32 index = (gRandomIndex + 1) & 1023;
        u32 nextIndex;
        const u8 *sample = (void *)(index * 2 + (u32)table);
        mask = 255;
        angle = *sample;
        nextIndex = (index + 1) & 1023;
        gRandomIndex = nextIndex;
        {
            s32 raw = *(const u16 *)(nextIndex * 2 + (u32)table);
            s32 bitmask = 31;
            raw &= bitmask;

            magnitude = raw;
        }
    }
    v = &velocity;
    speed = magnitude - 16;
    {
        const s16 *sine = gUnk_086149C4;
        s32 product = sine[(angle + 64) & mask] * speed;
        savedOrigin = origin;
        v->x = Truncate(product);
        v->y = 0;
        {
            s32 product = sine[angle] * speed;

            v->z = Truncate(product);
        }
    }
    {
        u32 offset;
        const u16 *source;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = savedOrigin + gRandomIndex * 2;
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
