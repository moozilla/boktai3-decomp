#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_UpdateRestPose(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_SetFacing(p, p->savedFacing);
        SceneActors_StartAnimation(p, &actor->animation, (int)&actor->animationResource, 37, 1, 4);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(p, 12);
    p->actionReset(p);
}
