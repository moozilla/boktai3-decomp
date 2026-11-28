#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActorAnimation_UpdateSwingLight(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 22;
        else
            sequence = 60;
        SceneActors_StartAnimation(p, &actor->animation, (int)&actor->animationResource, sequence,
                                   2, 4);
    }
    if (!p->actionTryUpdate(p) && !p->actionReset(p)) {
        if (actor->animation.status & 2)
            p->actionFinished = 1;
    }
}
