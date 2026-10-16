#ifndef GUARD_SCENE_PARTICLE_POOL_H
#define GUARD_SCENE_PARTICLE_POOL_H
#include "contact_actor.h"
/* Provisional eight-slot anchored particle pool at 080682D0; its renderer uses
 * the common 0x28-byte effect record, distinct from the preceding 32-slot pool. */
struct SceneParticleEffect {
    u32 flags;
    u8 unknown4[4];
    u16 tile;
    u8 width, height;
    u8 unknownC[3], priority;
    u8 unknown10[8];
    struct ContactPosition position;
    u8 unknown20[8];
};
struct SceneParticleSlot {
    const struct ContactPosition *anchor;
    struct ContactPosition offset;
    struct ContactPosition velocity;
    u8 unknown14[2];
    u8 state;
    s8 index;
    u16 frame;
    s16 delay;
};
struct SceneParticlePool {
    u8 unknown0[0x18];
    const void *sequenceResource;
    const void *graphicsResource;
    struct SceneParticleEffect effects[8];
    struct DisplaySequenceState sequences[8];
    u32 used;
    struct SceneParticleSlot slots[8];
};
struct SceneParticleSlot *SceneParticle_AllocateSlot(struct SceneParticlePool *);
#endif
