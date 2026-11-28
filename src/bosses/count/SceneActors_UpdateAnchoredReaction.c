#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_UpdateAnchoredReaction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;

    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 11;
        else
            sequence = 49;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        p->targetPosition = *(struct StateActorPos *)&body->position;
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    if (!p->actionReset(p))
        actor->graphics.reactionDelay = 6;
}
