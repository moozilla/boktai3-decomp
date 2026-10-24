#include "global.h"

// confidence=low: re-enters the current state (1..0x1D) through the same request path as
// Vanargand_ResetState (sub_081AF76C), or clears it; called on paired scene halves by sub_081B5D8C.
/* Actor state layout shared with Vanargand_ResetState. */
struct VanargandActor {
    u8 unknown0[0x160];
    u32 flags;
    u32 stateCounter;
    u32 animationCounter;
    u8 state;
    u8 requestedState;
    u8 stateEntered;
    u8 unknown16F;
    u8 transitionReady;
    u8 unknown171[3];
    s16 health;
    s16 maxHealth;
    u32 stateTimer;
};

static inline void EnterState(struct VanargandActor *actor, u8 state)
{
    u32 mask = 0x100;
    u32 blocked = actor->flags & mask;
    u8 *requested;
    u8 *ready;
    u8 current;
    if (blocked)
        return;
    requested = &actor->requestedState;
    current = *requested - 0x19;
    ready = &actor->transitionReady;
    if (current <= 1) {
        if (!*ready)
            return;
    }
    *requested = state;
    actor->state = state;
    actor->stateEntered = 1;
    *ready = blocked;
    actor->stateTimer = blocked;
    actor->stateCounter = blocked;
    actor->animationCounter = blocked;
}
void sub_081AFF98(struct VanargandActor *actor)
{
    if ((u8)(actor->state - 1) > 0x1c)
        actor->state = 0;
    else
        EnterState(actor, actor->state);
}
