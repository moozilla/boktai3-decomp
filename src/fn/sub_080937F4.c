#include "global.h"
void Bee_TickBehaviorCooldown(u8 *);
void sub_0809366C(u8 *);
void Bee_TickActionCooldown(u8 *);
u32 Bee_UpdateTimersAndContacts(u8 *p)
{
    Bee_TickBehaviorCooldown(p);
    sub_0809366C(p);
    Bee_TickActionCooldown(p);
    return 1;
}
