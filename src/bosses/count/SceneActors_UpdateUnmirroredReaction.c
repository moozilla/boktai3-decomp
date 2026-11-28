#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_UpdateUnmirroredReaction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;

    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 32;
        else
            sequence = 70;
        SceneActors_StartUnmirroredAnimation(p, &actor->animation, (int)&actor->animationResource,
                                             sequence, 1, 4);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    if (!p->actionReset(p))
        actor->graphics.reactionDelay = 8;
}
