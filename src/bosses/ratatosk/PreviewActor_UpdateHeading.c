#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void PreviewActor_UpdateHeading(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    Ratatosk_SetFacing(p, p->requestedAngle);
    if (Ratatosk_ConsumeActionEntry(p) == 1) {
        if (q->scaleMode == 0)
            Ratatosk_SetAnimation(p, 1);
        else
            Ratatosk_SetAnimation(p, 2);
    }
    if (p->actionFinished)
        q->sequenceDone = 1;
    p->actionTimer++;
}
