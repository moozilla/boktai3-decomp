#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
static inline s32 ConsumeActivation(struct Ratatosk *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
        return 1;
    }
    return 0;
}
void PreviewActor_UpdateHeading(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    p->body.direction = p->requestedAngle;
    {
        s32 angle = (p->body.direction + 0x60) >> 6;
        s32 mask = 3;
        mask &= angle;
        p->facing = mask;
    }
    {
        s32 angle = (p->body.direction + 0x70) >> 5;
        s32 mask = 7;
        mask &= angle;
        p->facing8 = mask;
    }
    if (ConsumeActivation(p) == 1) {
        if (q->scaleMode == 0)
            p->actionAnimation = 1;
        else
            Ratatosk_SetAnimation(p, 2);
    }
    if (p->actionFinished)
        q->sequenceDone = 1;
    p->actionTimer++;
}
