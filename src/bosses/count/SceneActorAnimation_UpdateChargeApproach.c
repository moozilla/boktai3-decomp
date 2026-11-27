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
    u8 *position = (u8 *)p + 0x48;
    SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        {
            int test = actor->graphics.animationFlags & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        {

            SceneActors_StartEightDirectionAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 1, 1, 4);
        }
        PolarInterpolation_SetCurrentFromPoint((u8 *)p + 0x30c, &p->target);
        sub_082222BC((u8 *)p + 0x30c, (u8 *)p + 0x9c4, actor->graphics.approachFrames);
        if (actor->graphics.approachFrames <= 64)
            Sound_PlaySongChecked(840);
        else
            Sound_PlaySongChecked(841);
    } else {
        int sequence;
        {
            int test = actor->graphics.animationFlags & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        {

            SceneActors_StartEightDirectionAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 1, 1, 1);
        }
    }
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 64);
        if (!p->actionReset(p)) {
            if (!(tick & 3)) {
                int id = SceneActorGraphics_GetResourceId(actor);
                {
                    u8 *data = gPaletteBankData + id * 32;
                    TemporaryAnimationPool_CreateEntry(&actor->animation, 8, 0x200, 0x9000, data);
                }
            }
            if (Interp_Step((u8 *)actor + 0x30c, position + 12))
                p->actionFinished = 1;
        }
    }
}
