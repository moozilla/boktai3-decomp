#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void BackgroundFlash_BeginAlternating(void *);

void SceneActorAnimation_UpdateFlashAlternatingRepeat(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        u8 cleared;
        p->actionChanged = 0;
        cleared = p->actionChanged;
        if (actor->graphics.animationFlags & 1)
            sequence = 28;
        else
            sequence = 66;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 0);
        SceneActors_UpdateAnimation(p, &actor->animation, (int)&actor->animationResource);
        BackgroundFlash_BeginAlternating(actor->graphics.backgroundFlash);
        if (actor->graphics.stepTableKind == 1)
            Sound_PlaySongChecked(900);
        Sound_PlaySongChecked(886);
        actor->graphics.backgroundFlashFlag = actor->graphics.initialFlashFlag;
        actor->graphics.flashPhase = cleared;
    }
    if (p->actionTryUpdate(p)) {
        if (tick <= 32)
            SceneActorGraphics_SetEffect(actor, 2, 826, 5);
    } else {
        SceneActorState_SetFlags(p, 0x40);
        if (!p->actionReset(p)) {
            if (tick == 32)
                SceneActorGraphics_SetEffect(actor, 2, 826, 5);
            if (actor->animation.status & 2)
                p->actionFinished = 1;
        }
    }
}
