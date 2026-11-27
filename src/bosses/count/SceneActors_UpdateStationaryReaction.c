#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_UpdateStationaryReaction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged)
        p->actionChanged = 0;
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(p, 28);
    {
        u8 result = p->actionReset(p);
        if (!result) {
            actor->graphics.scaleMin = result;
            actor->graphics.scaleMax = result;
            actor->variant = 0;
        }
    }
}
