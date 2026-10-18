#include "global.h"
#include "actor_state.h"
s32 ActorMovement_CanReachTarget(struct ActorState *);
s32 SceneVolume_GetActivityState(struct ActorState *);
void SceneVolume_BeginPhase(struct ActorState *, s32);
static inline u8 ConsumeTrigger(u8 *p)
{
    if (p[0xb0]) {
        p[0xb0] = 0;
        p[0xaf] = 0;
        return 1;
    }
    return 0;
}
void ActorState_UpdateDepletedState(struct ActorStateOwner *owner)
{
    struct ActorState *p = &owner->actor;
    if (ConsumeTrigger((u8 *)&owner->actor)) {
        u32 state = 1;
        owner->actor.state = state;
    }
    if (ActorMovement_CanReachTarget(p))
        SceneVolume_BeginPhase(&owner->actor, 1);
    else if (SceneVolume_GetActivityState(&owner->actor))
        SceneVolume_BeginPhase(&owner->actor, 2);
    else if (owner->actor.timer > 60) {
        owner->actor.requestedVariant = owner->actor.variant;
        owner->actor.request = 1;
        owner->actor.requestPhase = 0;
    }
    owner->actor.timer++;
}
