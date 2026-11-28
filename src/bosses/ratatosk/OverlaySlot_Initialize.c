#include "global.h"
#include "bosses/ratatosk/ratatosk_small_particle.h"

void SlotMotion_Initialize(struct ParticleConfig *);
void Sprite_InitUnlinked(struct ContactSwarmSprite *, void *, s32);

static inline void clearPosition(void *target, u32 *source, u32 flags)
{
    *source = 0;
    CpuSet(source, target, flags);
}

void OverlaySlot_Initialize(struct ParticlePool *pool, struct Particle *e)
{
    u32 zero;
    SlotMotion_Initialize(&e->config);
    e->age = 0;
    e->fallDistance = 0;
    e->fallSpeed = 0;
    clearPosition(&e->velocity, &zero, 0x05000002);
    Sprite_InitUnlinked(&e->sprite, &pool->graphics, 1);
}
