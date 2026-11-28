#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActorAnimation_UpdateImpactFinish(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    u8 reset;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 24;
        else
            sequence = 62;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 1, 1, 4);
        SceneActorGraphics_SetEffect(actor, 1, 827, 5);
        Sound_PlaySongChecked(838);
        Sound_PlaySongChecked(1334);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    reset = p->actionReset(p);
    if (!reset && tick > 31) {
        p->actionFinished = 1;
        actor->graphics.scaleMin = 0;
        actor->graphics.scaleMax = 0;
        actor->variant = 0;
    }
}
