#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "bosses/ratatosk/ratatosk_facing.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
static inline void SetFacing(struct Ratatosk *p, int angle)
{
    RATATOSK_SET_FACING(p, angle);
}

void sub_081F62A8(struct Ratatosk *p)
{
    struct Ratatosk *p2 = p;
    struct SceneActorBody *body = &p->body;
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 1);
    }
    Ratatosk_SetFlags(p2, 0xc);
    if ((u8)Callback_InvokeActor(p2, p2->actionReset) == 0) {
        int d = (s8)(body->direction - p2->savedAngle);
        int ad = d;
        if (ad < 0)
            ad = -ad;
        if (ad <= 7) {
            SetFacing(p2, p2->savedAngle);
            p2->actionFinished = 1;
        } else {
            int a;
            if (d < 0)
                a = body->direction + 8;
            else
                a = body->direction - 8;
            SetFacing(p2, a);
        }
        p->scaleX = 0;
        p->scaleZ = 0;
        p->scaleMode = 0;
    }
}
