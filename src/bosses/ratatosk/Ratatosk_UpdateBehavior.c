#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

extern void *const gUnk_08613F54[];
void Callback_InvokeActor(void *, void *);

void Ratatosk_UpdateBehavior(struct Ratatosk *p)
{
    p->behaviorFlag = 0;
    Callback_InvokeActor(p, gUnk_08613F54[p->actionMode]);
}
