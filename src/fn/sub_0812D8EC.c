#include "moving_particle_pool.h"

extern MovingParticleUpdate gUnk_08610808[];

u32 ParticleEffectPool_Update(struct MovingParticlePool *pool)
{
    struct MovingParticleEntry *start = pool->entries;
    struct MovingParticleEntry *entry;
    s32 i;
    u32 *occupied;
    u8 *state;
    MovingParticleUpdate *callbacks;
    if (!pool->occupied)
        return 0;
    i = 0;
    occupied = MovingParticle_SlotMask(pool);
    callbacks = gUnk_08610808;
    state = &pool->entries[0].state;
    entry = start;
    for (; i < 26; state += sizeof(*entry), entry++, i++) {
        if (MovingParticle_IsOccupied(occupied, i))
            callbacks[*state](pool, entry);
    }
    return 0;
}
