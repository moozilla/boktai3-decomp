// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
extern struct TetherActor *gUnk_02000488;
u32 TetherActor_CanStartInteraction(void)
{
    struct TetherActor *actor = gUnk_02000488;
    u32 result;
    if (!actor)
        goto no;
    if (actor->interactionLock)
        goto no;
    asm("" ::: "r2");
    if (actor->blendRequested)
        goto no;
    asm("" ::: "r2");
    if (actor->statusFlags)
        goto no;
    asm("" ::: "r2");
    if (actor->interactionCooldown)
        goto no;
    if (actor->state <= 1 || ((u8)(actor->state - 3) <= 1 && actor->pendingEffect))
        result = 1;
    else {
    no:
        result = 0;
    }
    return result;
}
