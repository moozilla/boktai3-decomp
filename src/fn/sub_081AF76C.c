#include "paired_scene_actor.h"

void Vanargand_ResetState(struct PairedSceneHalf *actor, s32 state)
{
    if (state == 1 || state == 0x1b) {
        if (actor->health <= 0)
            actor->health = actor->maxHealth;
        {
            u32 mask = 0x100;
            u32 blocked = actor->actionFlags & mask;
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
    }
}
