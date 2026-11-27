#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_control_helpers.h"
#include "player/player.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "engine/word_flags.h"

static inline void OrTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = bits;
    mask |= tracking->flags;
    tracking->flags = mask;
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
    SceneActorTerrain_EnterState(actor, 0, 2, 0);
    ActorShadow_Enable(actor->base.shadow);
    SceneActorControl_StartPositionCurve(actor, (const struct ContactPosition *)&gPlayers[0]->position);
}
