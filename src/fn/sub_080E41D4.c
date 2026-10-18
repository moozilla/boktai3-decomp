#include "global.h"

void sub_080DEA7C(void *);
void Dog_UpdateActionTimers(void *);
void Dog_UpdateActionCooldown(void *);
void Dog_TickCooldown(void *);

s32 Dog_FinishFrame(void *p)
{
    sub_080DEA7C(p);
    Dog_UpdateActionTimers(p);
    Dog_UpdateActionCooldown(p);
    Dog_TickCooldown(p);
    return 1;
}
