#include "global.h"

#define Vanargand_ResetState sub_081AF76C

/* Tentative actor state layout; health is also decremented by 081AF7F4. */
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

void sub_081AF76C(struct VanargandActor *actor, s32 state)
{
    if (state == 1 || state == 0x1b) {
        if (actor->health <= 0)
            actor->health = actor->maxHealth;
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
    }
}
