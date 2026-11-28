#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActorGraphics_SelectResource(void *);

void SceneActors_UpdateShrinkingReaction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        SceneActors_SetFacing(p, p->bodyDirection);
        if (actor->graphics.animationFlags & 1)
            sequence = 11;
        else
            sequence = 49;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        SceneActorGraphics_SelectResource(p);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    SceneActorState_SetFlags(p, 0x40);
    if (actor->graphics.scaleMin > 23)
        actor->graphics.scaleMin -= 24;
    else
        actor->graphics.scaleMin = 0;
    if (actor->graphics.scaleMax > 23)
        actor->graphics.scaleMax -= 24;
    else
        actor->graphics.scaleMax = 0;
}
