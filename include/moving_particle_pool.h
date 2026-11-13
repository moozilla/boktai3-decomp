#ifndef GUARD_MOVING_PARTICLE_POOL_H
#define GUARD_MOVING_PARTICLE_POOL_H
#include "engine.h"
#include "scene_particle_pool.h"

/* Both moving-particle task families dispatch by the state at 0x32. */
struct MovingParticleEntry {
    struct SceneParticleEffect sprite;
    struct ContactPosition velocity;
    u16 motionFlags;
    u8 state;
    s8 index;
    u8 reset, initialDelay;
    u8 animationTimer, animationFrame;
    u16 lifetime, cell;
    u8 frame, unknown3D[3];
};
struct MovingParticlePool {
    struct Task task;
    void *resource;
    struct MovingParticleEntry entries[26];
    u32 occupied;
};
static inline u32 *MovingParticle_SlotMask(struct MovingParticlePool *pool)
{
    return &pool->occupied;
}
static inline u8 MovingParticle_IsOccupied(u32 *occupied, s32 index)
{
    if (*occupied & (1 << index))
        return TRUE;
    return FALSE;
}
typedef void (*MovingParticleUpdate)(struct MovingParticlePool *, struct MovingParticleEntry *);

#endif
