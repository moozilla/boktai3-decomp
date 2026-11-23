#include "global.h"
#include "ratatosk.h"
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void SceneActorAnimation_UpdateSwoopMotion(struct Ratatosk *p, s32 tick)
{
    struct Ratatosk *q = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked) {
            int angle = p->savedAngle;
            Ratatosk_SetFacing(p, angle);
        }
        if (q->scaleMode == 0) {
            q->scaleMode = 1;
            q->scaleSpeed = 0x20;
            SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 2, 1, 4);
        } else {
            q->scaleMode = 0;
            q->scaleSpeed = -32;
            SceneActors_StartEightDirectionAnimation(p, p->animationData, p->animation, 2, 1, 1, 4);
        }
    }
    if ((u8)Callback_InvokeActor(p, p->actionTryUpdate))
        return;
    if (q->scaleMode == 0) {
        q->scaleX = q->scaleX * 3 >> 2;
        q->scaleZ = q->scaleZ * 0x3a >> 6;
    } else {
        q->scaleX = (q->scaleX * 0x3a + 0x1680) >> 6;
        q->scaleZ = (q->scaleZ * 0x30 + 0x5a00) >> 6;
    }
    if (q->scaleMode == 1) {
        if (tick > 9)
            SceneActors_StartEightDirectionAnimation(p, q->animationData, q->animation, 2, 2, 1, 4);
        else
            SceneActors_StartDirectionalAnimation(p, q->animationData, q->animation, 0, 2, 1, 1);
    } else if (q->distance > 0x13f) {
        SceneActors_StartEightDirectionAnimation(p, q->animationData, q->animation, 2, 2, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, q->animationData, q->animation, 0, 2, 1, 1);
    }
    Ratatosk_SetFlags(p, 0x40);
    if (tick > 0x2f) {
        if (q->scaleMode == 0) {
            q->scaleX = 0;
            q->scaleZ = 0;
        } else {
            q->scaleX = 960;
            q->scaleZ = 1440;
        }
        p->actionFinished = 1;
    }
}
