#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

extern void *const gUnk_08613F4C[];
void Callback_InvokeActor(void *, void *);

void Ratatosk_UpdateInactiveMode(struct Ratatosk *p)
{
    Callback_InvokeActor(p, gUnk_08613F4C[p->actionIndex]);
}
