#include "scene_actors.h"
#include "scene_graphics_actor.h"

void SceneActors_UpdateReleasePose(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_SetFacing(p, p->savedFacing);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              72, 2, 1, 4);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    {
        u32 mask = 12;
        p->actionFlags |= mask;
    }
    if (p->animationFinished) {
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              5, 2, 1, 0);
        p->actionFinished = 1;
    }
    p->actionReset(p);
}
