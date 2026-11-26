#include "actors/common_scene_actor_controller/scene_actors.h"
#include "engine/shared/scene_graphics_actor.h"

void SceneActorAnimation_ResetActionState(struct StateActor *p)
{
    if (p->actionChanged)
        p->actionChanged = 0;
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(p, 12);
    SceneActorState_SetFlags(p, 0x10010);
    p->actionReset(p);
}
