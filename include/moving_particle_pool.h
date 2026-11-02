#ifndef GUARD_MOVING_PARTICLE_POOL_H
#define GUARD_MOVING_PARTICLE_POOL_H
#include "global.h"

/* Both moving-particle task families dispatch by the state at 0x32. */
struct MovingParticleEntry {
    u8 unknown0[0x32];
    u8 state;
    u8 unknown33[0xD];
};
struct MovingParticlePool {
    u8 unknown0[0x1C];
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
