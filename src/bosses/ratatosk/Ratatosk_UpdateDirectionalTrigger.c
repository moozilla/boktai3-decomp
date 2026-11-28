#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

/* Same as SceneActor_UpdateDirectionalTrigger, but marks the sequence done instead of consuming a
 * trigger (provisional name). */
void Ratatosk_UpdateDirectionalTrigger(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    Ratatosk_SetFacing(p, p->requestedAngle);
    if (Ratatosk_ConsumeActionEntry(p)) {
        if (q->scaleMode == 0)
            Ratatosk_SetAnimation(p, 3);
        else
            Ratatosk_SetAnimation(p, 4);
    }
    if (p->actionFinished) {
        if (q->actionState != 3) {
            q->sequenceDone = 1;
            return;
        }
        Ratatosk_SetAnimation(q, 4);
    }
    p->actionTimer++;
}
