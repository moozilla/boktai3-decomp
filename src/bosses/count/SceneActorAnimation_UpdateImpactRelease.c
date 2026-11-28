#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActorAnimation_UpdateImpactRelease(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 24;
        else
            sequence = 62;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 1, 1, 6);
        actor->graphics.scaleMin = 0;
        actor->graphics.scaleMax = 0;
        actor->variant = 0;
        SceneActorGraphics_SetEffect(actor, 2, 827, 6);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    if (!p->actionReset(p) && (actor->animation.status & 2))
        p->actionFinished = 1;
}
