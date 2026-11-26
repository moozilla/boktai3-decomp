#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"

void Ratatosk_UpdateTimedState16(struct Ratatosk *p)
{
    if (Ratatosk_ConsumeActionEntry(p))
        Ratatosk_SetAnimation(p, 0x10);
    p->actionTimer++;
}
