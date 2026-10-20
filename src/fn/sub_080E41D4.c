#include "global.h"

void Dog_UpdateCollisionBounds(void *);
void Dog_UpdateActionTimers(void *);
void Dog_UpdateActionCooldown(void *);
void Dog_TickCooldown(void *);

s32 Dog_FinishFrame(void *p)
{
    Dog_UpdateCollisionBounds(p);
    Dog_UpdateActionTimers(p);
    Dog_UpdateActionCooldown(p);
    Dog_TickCooldown(p);
    return 1;
}
