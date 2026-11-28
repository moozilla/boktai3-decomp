#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"
#include "engine/video/render.h"

void PolarInterpolation_SetCurrentFromPoint(void *, void *);
void sub_082222BC(void *, void *, int);
int Interp_Step(void *, void *);
int SceneActorGraphics_GetResourceId(void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);

void SceneActorAnimation_UpdateChargeApproach(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
    SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 0;
        else
            sequence = 38;
        SceneActors_StartEightDirectionAnimation(p, &actor->animation,
                                                 &actor->animationResource, sequence, 1, 1, 4);
        PolarInterpolation_SetCurrentFromPoint(actor->approachMotion, &body->position);
        sub_082222BC(actor->approachMotion, &actor->graphics.snapPosition,
                     actor->graphics.approachFrames);
        if (actor->graphics.approachFrames <= 64)
            Sound_PlaySongChecked(840);
        else
            Sound_PlaySongChecked(841);
    } else {
        int sequence;
        if (actor->graphics.animationFlags & 1)
            sequence = 0;
        else
            sequence = 38;
        SceneActors_StartEightDirectionAnimation(p, &actor->animation,
                                                 &actor->animationResource, sequence, 1, 1, 1);
    }
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 64);
        if (!p->actionReset(p)) {
            if (!(tick & 3))
                TemporaryAnimationPool_CreateEntry(
                    &actor->animation, 8, 0x200, 0x9000,
                    gPaletteBankData + SceneActorGraphics_GetResourceId(actor) * 32);
            if (Interp_Step(actor->approachMotion, &body->position))
                p->actionFinished = 1;
        }
    }
}
