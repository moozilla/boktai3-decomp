#ifndef GUARD_SCENE_PARTICLE_BURST_H
#define GUARD_SCENE_PARTICLE_BURST_H
#include "scene_particle_pool.h"
#include "task.h"
/* Tentative burst-particle pool: independent positions damp velocity by 7/10. */
struct SceneParticleBurstSlot {
    struct ContactPosition position;
    struct ContactPosition velocity;
    u16 flags;
    u8 state;
    s8 index;
    u16 frame;
    s16 delay;
};
struct SceneParticleBurstPool {
    struct Task task;
    const void *sequenceResource;
    const void *graphicsResource;
    struct SceneParticleEffect effects[32];
    struct DisplaySequenceState sequences[32];
    u32 used;
    u32 mode;
    struct SceneParticleBurstSlot slots[32];
};
#endif
