#include "directional_particles.h"
#include "scene_actor_registry.h"
#include "engine.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"

s32 DirectionalParticle_Update(void *, struct DirectionalParticle *);
static inline s32 scaleVelocity(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
s32 DirectionalParticle_Spawn(struct DirectionalParticleSource *source, u32 direction, u32 frame)
{
    struct DirectionalParticle *particle = SpritePool_Allocate();
    struct DirectionalParticleMotion *motion;
    union SceneActorRegistryPackedPosition position;
    struct ContactPosition *point;
    s32 speed;
    u32 randomOrigin, sampleOrigin;
    const u16 *earlyTable;
    if (!particle)
        return -1;
    motion = &particle->motion;
    /* The origin cancels when sample addresses become offsets in the physical table. */
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    earlyTable = (const u16 *)RANDOM_TABLE_ADDR;
    motion->lifetime = (ParticleRandom_ReadDirectSample(earlyTable) & 7) + 10;
    speed = (ParticleRandom_ReadDirectSample(earlyTable) & 7) + 24;
    {
        s32 product = gUnk_086149C4[(direction + 64) & 255] * speed;
        sampleOrigin = randomOrigin;
        motion->velocity.x = scaleVelocity(product);
    }
    motion->velocity.y = 0;
    motion->velocity.z = scaleVelocity(gUnk_086149C4[direction & 255] * speed);
    point = &position.position;
    *point = *source->position;
    {
        u32 sample = ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable);
        s32 coordinate = (u16)point->x - 64;
        position.packed.x = coordinate + (sample & 127);
    }
    {
        u32 sample = ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable);
        s32 coordinate = (u16)point->y + 16;
        position.packed.y = coordinate + (sample & 31);
    }
    {
        u32 sample = ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable);
        s32 coordinate = (u16)point->z - 64;
        position.packed.z = coordinate + (sample & 127);
    }
    particle->visible = 1;
    SpritePool_SetResource(particle, 0x4147);
    SpritePool_SetFrame(particle, frame);
    SpritePool_SetPalette(particle, 0x164);
    SpritePool_SetPosition(particle, point);
    SpritePool_Activate(particle, (void *)DirectionalParticle_Update, source);
    return 0;
}
