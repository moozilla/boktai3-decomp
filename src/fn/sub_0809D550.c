#include "global.h"

void sub_0809C91C(u8 *);
void RouteActor_UpdateRandomDestination(u8 *);

u32 Lun_UpdateMovement(u8 *p)
{
    sub_0809C91C(p);
    RouteActor_UpdateRandomDestination(p);
    return 1;
}
