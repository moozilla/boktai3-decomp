#define gRandomValues gUnk_086141BC
#include "global.h"
#include "scene_object.h"
#include "ram.h"
extern const u16 gUnk_086141BC[];
struct ActorParticleEmitter {
    u8 interval, countdown, count, enabled;
};
u32 ActorParticles_CreateAtOffset(void *, struct SceneExtent *, struct SceneExtent *, u32, u32, u32,
                                  u32, u32, u32, u32, u32, u32, u32, u32, u32);
static inline u32 NextRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
void ActorParticles_UpdateEmitter(void *actor, struct ActorParticleEmitter *emitter)
{
    struct SceneExtent offset, velocity;
    s32 i;
    if (!emitter->enabled)
        return;
    if (emitter->countdown) {
        emitter->countdown--;
        return;
    }
    emitter->countdown = emitter->interval;
    for (i = 0; i < emitter->count; i++) {
        offset.x = (NextRandom() & 31) - 7;
        offset.y = 0;
        offset.z = 0;
        velocity.x = (NextRandom() & 1) + 1;
        velocity.y = -(NextRandom() & 1);
        velocity.z = 0;
        ActorParticles_CreateAtOffset(actor, &offset, &velocity, 0x4384, 0xf62b, 0, 0, 0, 0, 0xb8,
                                      30, 0, !(i & 1) ? 512 : 1024, 1, 0);
    }
}
