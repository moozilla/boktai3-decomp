#ifndef GUARD_SCENE_MOVING_PARTICLE_H
#define GUARD_SCENE_MOVING_PARTICLE_H
#include "moving_particle_pool.h"
struct MovingParticleEntry *SceneMovingParticle_AllocateEntry(struct MovingParticlePool *);
void SceneMovingParticle_UpdateAnimation(struct MovingParticlePool *, struct MovingParticleEntry *);
struct MovingParticlePool *SceneMovingParticle_EnsurePool(void);
s32 SceneMovingParticle_InitPool(struct MovingParticlePool *);
u32 SceneMovingParticle_OnPoolDestroy(struct MovingParticlePool *);
u32 SceneMovingParticle_UpdatePool(struct MovingParticlePool *);
struct MovingParticleEntry *SpriteBurst_Create(const struct ContactPosition *,
                                               const struct ContactPosition *, u32, u32, u32);
#endif
