#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

extern void *const gUnk_08613F34[];
void Callback_InvokeActor(void *, void *);

void Ratatosk_UpdateInactiveState(struct Ratatosk *p)
{
    Callback_InvokeActor(p, gUnk_08613F34[p->actionPhase]);
}
