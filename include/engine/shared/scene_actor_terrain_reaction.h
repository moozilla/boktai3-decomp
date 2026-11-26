#ifndef GUARD_SCENE_ACTOR_TERRAIN_REACTION_H
#define GUARD_SCENE_ACTOR_TERRAIN_REACTION_H
#include "engine/shared/scene_actor_state.h"

static inline u8 SceneActorTerrain_ExcludesCategory(u32 category)
{
    u32 relative = category - 0x11;
    if (relative <= 3 || category == 0x17)
        return 1;
    return 0;
}
static inline void SceneActorTerrain_InstallControl(struct ChaseActor *actor, void *sink, u8 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = sink;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void SceneActorTerrain_EnterState(struct ChaseActor *actor, u8 state, u8 phase,
                                                u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

#endif
