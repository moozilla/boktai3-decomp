#include "bosses/vanargand/paired_scene_actor.h"
#include "global.h"

// confidence=low: re-enters the current state (1..0x1D) through the same request path as
// Vanargand_ResetState (sub_081AF76C), or clears it; called on paired scene halves by sub_081B5D8C.
/* Actor state layout shared with Vanargand_ResetState. */

static inline void EnterState(struct PairedSceneHalf *actor, u8 state)
{
    u32 blocked = PairedSceneHalf_TestActionFlags(actor, 0x100);
    u8 *requested;
    u8 *ready;
    u8 current;
    if (blocked)
        return;
    requested = &actor->requestedState;
    current = *requested - 0x19;
    ready = &actor->finished;
    if (current <= 1) {
        if (!*ready)
            return;
    }
    *requested = state;
    actor->state = state;
    actor->entered = 1;
    *ready = blocked;
    actor->stateTimer = blocked;
    actor->actionCounters[0] = blocked;
    actor->actionCounters[1] = blocked;
}
void sub_081AFF98(struct PairedSceneHalf *actor)
{
    if ((u8)(actor->state - 1) > 0x1c)
        actor->state = 0;
    else
        EnterState(actor, actor->state);
}
