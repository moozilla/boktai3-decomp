#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void Ratatosk_UpdateEightDirectionAnimation(struct Ratatosk *p, s32 frame)
{
    struct Ratatosk *q = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked) {
            int angle = p->savedAngle;
            Ratatosk_SetFacing(p, angle);
        }
        SceneActors_StartEightDirectionAnimation(p, p->animationData, p->animation, 2, 1, 1, 4);
    } else
        SceneActors_StartEightDirectionAnimation(p, p->animationData, p->animation, 2, 1, 1, 1);
    q->scaleX = 960;
    q->scaleZ = 1440;
    q->scaleMode = 1;
    Ratatosk_SetFlags(p, 0x100000);
    if (!(u8)Callback_InvokeActor(p, p->actionTryUpdate)) {
        Ratatosk_SetFlags(p, 0x40);
        if (frame >= q->interval && !p->locked)
            p->actionFinished = 1;
    }
}
