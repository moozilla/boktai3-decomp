// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
void TetherActor_UpdateInteractionCooldown(struct TetherActor *actor)
{
    u16 *field = &actor->interactionCooldown;
    u32 timer = *field;
    register u32 saved asm("r3") = timer;
    register u32 result asm("r0");
    if (!saved)
        goto done;
    if (actor->state != 1)
        goto other;
    if (saved <= 2)
        goto zero;
    result = timer - 2;
    goto store;
zero:
    result = 0;
    goto store;
other:
    result = timer - 1;
store:
    *field = result;
done:;
}
