#include "global.h"
#include "ratatosk_particle.h"

void Sprite_InitUnlinked(struct DisplaySequenceSprite *, void *, s32);
static inline void clearPosition(void *target, u32 *source, u32 flags)
{
    *source = 0;
    CpuSet(source, target, flags);
}
void OverlaySlot_InitializeExtended(struct RatatoskParticlePool *manager,
                                    struct RatatoskParticle *slot, s32 index)
{
    u32 zero;
    Ratatosk_InitAnimatedParticleParams(&slot->config);
    slot->age = 0;
    slot->fallDistance = 0;
    slot->fallSpeed = 0;
    clearPosition(&slot->velocity, &zero, 0x05000002);
    Sprite_InitUnlinked(&slot->sprite, &manager->graphics, 1);
}
