#include "global.h"

void Lun_UpdateHoverHeight(u8 *);
void RouteActor_UpdateRandomDestination(u8 *);

u32 Lun_UpdateMovement(u8 *p)
{
    Lun_UpdateHoverHeight(p);
    RouteActor_UpdateRandomDestination(p);
    return 1;
}
