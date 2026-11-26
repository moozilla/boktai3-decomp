#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/shared/render.h"
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void PolarInterpolation_SetCurrentFromPoint(void *, void *);
void sub_082222BC(void *, void *, int);
int Interp_Step(void *, void *);
void Sound_PlaySong(int);
int ActorResource_GetFixedId(void *);
int Callback_InvokeActor(void *, void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);
void SceneActorAnimation_UpdateDiveApproach(struct Ratatosk *p, int tick)
{
    struct Ratatosk *p2 = p;
    struct SceneActorBody *body = &p->body;
    Ratatosk_SetFacing(p, p->requestedAngle);
    Ratatosk_SetFlags(p, 8);
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0x14, 1, 4, 4);
        PolarInterpolation_SetCurrentFromPoint(&p->motionPath, &p->body.position);
        {
            sub_082222BC(&p->motionPath, &p->destination, p->duration);
            if (p->actionPhase != 5) {
                if (p->duration <= 64)
                    Sound_PlaySong(0x567);
                else
                    Sound_PlaySong(0x56c);
            }
        }
    } else {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0x14, 1, 4, 1);
    }
    if (!(u8)Callback_InvokeActor(p, p->actionTryUpdate)) {
        if (!(tick & 3)) {
            int id = ActorResource_GetFixedId(p2);
            {
                u8 *data = gPaletteBankData + id * 32;
                TemporaryAnimationPool_CreateEntry(p2->animationData, 8, 0x200, 0xe011, data);
            }
        }
        if (p2->duration == 0x20)
            Ratatosk_SetFlags(p, 0x40000);
        if (p2->duration <= 0x40)
            Ratatosk_SetFlags(p, 0x40);
        if (Interp_Step(&p2->motionPath, &body->position))
            p->actionFinished = 1;
    }
}
