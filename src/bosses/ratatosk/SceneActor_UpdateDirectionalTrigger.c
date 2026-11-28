#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

u32 Trigger_ConsumePending(void *latch, u32 expected);

void SceneActor_UpdateDirectionalTrigger(struct Ratatosk *p)
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
            Trigger_ConsumePending(p->receiver2, 1);
            return;
        }
        Ratatosk_SetAnimation(q, 4);
    }
    p->actionTimer++;
}
