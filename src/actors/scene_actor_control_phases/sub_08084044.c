#include "engine/shared/scene_actor_control_helpers.h"
#include "engine/shared/player.h"
#include "actors/scene_actor_reaction/scene_actor_reaction.h"
#include "engine/shared/scene_actor_chase_shared.h"
#include "engine/shared/word_flags.h"

static inline void OrTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = bits;
    mask |= tracking->flags;
    tracking->flags = mask;
}

static inline void SelectState(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
}

void SceneActorControl_RestartPositionCurve(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);

    state->phase = 0;
    ZeroFill8(&actor->base.secondaryMotion);
    SceneActorChase_AndWordFlags(&state->flags3D4, ~7);
    OrTrackingFlags(&actor->base.tracking, 8);
    WordFlags_Set(0x100000, &actor->base.statusFlags);
    SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
    SelectState(actor, 0, 2, 0);
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
    ActorShadow_Enable(actor->base.shadow);
    SceneActorControl_StartPositionCurve(actor, &gPlayers[0]->position);
}
