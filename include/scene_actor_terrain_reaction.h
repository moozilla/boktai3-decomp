#ifndef GUARD_SCENE_ACTOR_TERRAIN_REACTION_H
#define GUARD_SCENE_ACTOR_TERRAIN_REACTION_H
#include "scene_actor_state.h"

static inline u8 SceneActorTerrain_ExcludesCategory(u32 category)
{
    u32 relative = category - 0x11;
    if (relative <= 3 || category == 0x17)
        return 1;
    return 0;
}
static inline void SceneActorTerrain_InstallControl(struct SceneActorRegistryResetView *actor,
                                                    void *sink, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = sink;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SceneActorTerrain_EnterState(struct SceneActorRegistryResetView *actor, u8 state,
                                                u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

#endif
