#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void Ratatosk_UpdateWaitTimer(struct Ratatosk *p)
{
    Ratatosk_ConsumeActionEntry(p);
    p->actionTimer++;
}
