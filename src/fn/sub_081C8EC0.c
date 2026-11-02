#include "monster_particle_random.h"
#include "resource_registry.h"
#include "heap.h"
#include "state_actor.h"
#include "engine.h"
extern const s16 gUnk_086149C4[];
void *SpriteEffectController_Create(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                    u32, u32, u32, u32, u32, s32, s32);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
// confidence=low: emits a radial effect around the actor tag's position.
#define StateActor_SpawnRadialEffect sub_081C8EC0
void StateActor_SpawnRadialEffect(struct StateActor *actor)
{
    struct StateActorPos velocity;
    struct StateActorPos *v;
    struct StateActorTag *tag = &actor->tag;
    u32 randomOrigin, sampleOrigin;
    s32 heading, bits, speed, frames;
    const u16 *table;
    randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    table = gRandomTable;
    heading = (u8)ParticleRandom_ReadDirectSample(table);
    bits = (s32)ParticleRandom_ReadDirectSample(table);
    bits = (bits >> 4) & 63;
    v = &velocity;
    speed = bits + 32;
    sampleOrigin = randomOrigin;
    v->x = Scale(gUnk_086149C4[(heading + 64) & 255] * speed);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[heading] * speed);
    frames = (s32)ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR);
    frames = ((frames >> 4) & 15) + 7;
    SpriteEffectController_Create((u8 *)tag + 8, &velocity, 0x1C1E, 0xD43F, 0, 0, 17, 0, 2, 6, 0,
                                  frames, 0, 0, 0, 0, -4, -4);
}
