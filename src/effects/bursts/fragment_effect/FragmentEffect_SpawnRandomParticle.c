#include "effects/bursts/spiral_burst.h"
#include "engine/ram.h"
#include "engine/heap.h"
#include "engine/resource_registry.h"
#include "engine/math/monster_particle_random.h"

extern const s16 gUnk_086149C4[];
void DebrisMote_Create(struct ContactPosition *, struct ContactPosition *, s32, s32);

void FragmentEffect_SpawnRandomParticle(struct ContactPosition *position)
{
    struct ContactPosition velocity;
    struct ContactPosition *planar;
    /* The recovered bank origin cancels when converting a sample address to its table offset. */
    u32 randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    u32 sampleOrigin;
    s32 heading = ParticleRandom_ReadDirectSample(gRandomTable) & 255;
    s32 speed;
    {
        s32 raw = ParticleRandom_ReadDirectSample(gRandomTable) & 31;

        planar = &velocity;
        speed = raw - 16;
    }
    {
        s32 x = gUnk_086149C4[(heading + 64) & 255] * speed;

        sampleOrigin = randomOrigin;
        planar->x = SpiralBurst_ScaleOffset(x);
    }
    planar->y = 0;
    planar->z = SpiralBurst_ScaleOffset(gUnk_086149C4[heading] * speed);
    velocity.y =
        (ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR) & 63) - 48;
    DebrisMote_Create(position, &velocity, 60, 3);
}
