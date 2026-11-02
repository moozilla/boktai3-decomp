#include "scene_steering_actor.h"
#include "engine.h"
#include "ram.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"
extern const s16 gUnk_086149C4[];
void SceneMovingParticle_Create(struct ContactPosition *, struct ContactPosition *, s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void SceneSlot_EmitPulse(struct ContactPosition *position)
{
    struct ContactPosition velocity;
    struct ContactPosition *planar;
    u32 randomOrigin;
    u32 mask, sampleOrigin;
    s32 heading;
    s32 speed, vertical;

    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    mask = 255;
    heading = ParticleRandom_ReadDirectSample(gRandomTable) & mask;
    {
        s32 raw = ParticleRandom_ReadDirectSample(gRandomTable);
        s32 speedMask = 63;
        raw &= speedMask;
        planar = &velocity;
        speed = raw - 32;
    }
    {
        s32 x, zero, rawX;
        rawX = gUnk_086149C4[(heading + 64) & mask] * speed;
        sampleOrigin = randomOrigin;
        x = Truncate(rawX);
        zero = 0;
        planar->x = x;
        planar->y = zero;
        planar->z = Truncate(gUnk_086149C4[heading] * speed);
    }
    vertical = ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
    vertical &= 15;
    vertical += 30;
    velocity.y = vertical;
    SceneMovingParticle_Create(position, &velocity, 30, 3);
}
