#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_StopBackgroundFlash(void *);

void SceneActorAnimation_UpdateFlashPulse(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 28;
        else
            sequence = 66;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 0);
        actor->animation.frameIndex = 2;
        SceneActors_UpdateAnimation(p, &actor->animation, (int)&actor->animationResource);
        SceneActorGraphics_SetEffect(actor, 2, 826, 5);
        SceneActors_StopBackgroundFlash(actor);
    }
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        if (!p->actionReset(p) && tick > 30)
            p->actionFinished = 1;
    }
}
