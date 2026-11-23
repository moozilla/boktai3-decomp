#include "scene_actors.h"
#include "scene_graphics_actor.h"

void SceneActors_UpdateSpecialPose(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_SetFacing(p, p->savedFacing);
        SceneActors_StartAnimation(p, &actor->animation, (int)&actor->animationResource, 71, 2, 4);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(p, 12);
    p->actionReset(p);
}
