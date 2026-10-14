#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
#define TetherActor_UpdateSecondaryLock sub_0804BAE4
void TetherActor_UpdateSecondaryLock(struct TetherActor *input)
{
    register struct TetherActor *actor asm("r2") = input;
    if (!actor->statusFlags) {
        u16 *value = &actor->secondaryValue;
        u32 loaded = *value;
        register u32 current asm("r3");
        current = loaded;
        if (current)
            *value = loaded - 1;
        else {
            *value = actor->secondaryMaximum;
            actor->interactionLock = 1;
            {
                register u16 *frame asm("r0") = &actor->lockFrame;
                *frame = current;
            }
        }
    }
}
