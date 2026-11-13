#ifndef GUARD_NATIVE_PARTICLE_H
#define GUARD_NATIVE_PARTICLE_H
#include "scene_particle_pool.h"
#include "engine.h"
struct NativeParticleState {
    u16 flags;
    u8 state;
    s8 index;
    u16 timer, unknown16;
    u32 active;
};
struct NativeParticle {
    struct ContactPosition position, velocity;
    struct NativeParticleState control;
};
struct ParticleManager {
    struct Task task;
    void *resource;
    const u16 *graphics;
    struct SceneParticleEffect sprites[32];
    struct DisplaySequenceState sequences[32];
    struct NativeParticle entries[32];
};
struct NativeParticle *NativeParticle_AllocateEntry(struct ParticleManager *);
struct ParticleManager *EffectPool_CreateManager(void);
void NativeParticle_LoadResources(struct ParticleManager *);
s32 EffectPool_InitManager(struct ParticleManager *);
s32 EffectPool_UpdateEntries(struct ParticleManager *);
s32 NativeParticle_DestroyManager(struct ParticleManager *);
#endif
