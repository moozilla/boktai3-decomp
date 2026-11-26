#include "scene/motion/scene_steering_actor.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
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
            raw &= 31;

            magnitude = raw;
        }
    }
    v = &velocity;
    speed = magnitude - 16;
    {
        s32 product = gUnk_086149C4[(angle + 64) & mask] * speed;
        savedOrigin = origin;
        v->x = Truncate(product);
        v->y = 0;
        {
            s32 product = gUnk_086149C4[angle] * speed;

            v->z = Truncate(product);
        }
    }
    velocity.y =
        (ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR) & 63) - 48;
    SceneMovingParticle_Create(position, &velocity, 60, 3);
}
