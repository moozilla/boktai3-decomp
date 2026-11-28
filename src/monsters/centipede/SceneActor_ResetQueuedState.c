#include "global.h"
void SceneActorControl_OnIdle(void);
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
void SceneActor_ResetQueuedState(struct ChaseActor *s)
{
    SceneActorTerrain_EnterState(s, 0, 2, 0);
    SceneActorChase_SetControl(s, SceneActorControl_OnIdle, 16);
}
