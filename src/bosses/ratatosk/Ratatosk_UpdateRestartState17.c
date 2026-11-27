#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void Ratatosk_UpdateRestartState17(struct Ratatosk *p)
{
    if (Ratatosk_ConsumeActionEntry(p))
        Ratatosk_SetAnimation(p, 0x11);
    if (p->actionFinished)
        Ratatosk_StartAction(p, 0, 0, 1);
    p->actionTimer++;
}
