#include "scene_actor_control_helpers.h"
#include "player.h"
#include "scene_actor_reaction.h"

static inline void AndWordFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}

static inline void OrTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = bits;
    mask |= tracking->flags;
    tracking->flags = mask;
}

static inline void OrWordFlags(u32 mask, u32 *flags)
{
    *flags |= mask;
}

static inline void SelectState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                               u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
}

void SceneActorControl_RestartPositionCurve(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);

    state->phase = 0;
    ZeroFill8(&actor->secondaryMotion);
    AndWordFlags(&state->flags3D4, ~7);
    OrTrackingFlags(&actor->tracking, 8);
    OrWordFlags(0x100000, &actor->flags2D0);
    SceneActorReaction_EnableMotionFlags(0x100, &actor->flags2D4);
    SelectState(actor, 0, 2, 0);
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    ActorShadow_Enable(actor->shadow);
    SceneActorControl_StartPositionCurve(actor, &gPlayers[0]->position);
}
