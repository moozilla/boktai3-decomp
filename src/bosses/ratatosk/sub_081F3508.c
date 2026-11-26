#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void Ratatosk_UpdateDirectionalAnimation(struct Ratatosk *p, s32 frame)
{
    struct Ratatosk *q = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked) {
            int angle = p->savedAngle;
            Ratatosk_SetFacing(p, angle);
        }
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 4);
    } else
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 1);
    q->scaleX = 0;
    q->scaleZ = 0;
    q->scaleMode = 0;
    Ratatosk_SetFlags(p, 0x100000);
    if (!(u8)Callback_InvokeActor(p, p->actionTryUpdate)) {
        if (frame >= q->interval && !p->locked)
            p->actionFinished = 1;
        if (!p->actionPhase)
            Ratatosk_SetFlags(p, 0x10);
        Ratatosk_SetFlags(p, 0x40);
    }
}
