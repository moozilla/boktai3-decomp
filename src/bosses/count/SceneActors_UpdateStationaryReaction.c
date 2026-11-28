#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_UpdateStationaryReaction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged)
        p->actionChanged = 0;
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 28);
    if (!p->actionReset(p)) {
        actor->graphics.scaleMin = 0;
        actor->graphics.scaleMax = 0;
        actor->variant = 0;
    }
}
