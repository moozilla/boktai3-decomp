#ifndef GUARD_SCENE_MOVING_PARTICLE_H
#define GUARD_SCENE_MOVING_PARTICLE_H
#include "effects/particles/moving_particle_pool.h"
struct MovingParticleEntry *DebrisMote_AllocateEntry(struct MovingParticlePool *);
void DebrisMote_UpdateAnimation(struct MovingParticlePool *, struct MovingParticleEntry *);
struct MovingParticlePool *DebrisMote_EnsurePool(void);
s32 DebrisMote_InitPool(struct MovingParticlePool *);
u32 DebrisMote_OnPoolDestroy(struct MovingParticlePool *);
u32 DebrisMote_UpdatePool(struct MovingParticlePool *);
struct MovingParticleEntry *SpriteBurst_Create(const struct ContactPosition *,
                                               const struct ContactPosition *, u32, u32, u32);
#endif
