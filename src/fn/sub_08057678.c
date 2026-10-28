#include "proximity_icon.h"
s32 ProximityIcon_IsPlayerNearby(struct ProximityIcon *icon)
{
    s32 dx, dz;
    if (!gPlayers[0])
        return 0;
    dx = gPlayers[0]->position.x - icon->origin.x;
    if (dx < 0)
        dx = -dx;
    dz = gPlayers[0]->position.z - icon->origin.z;
    if (dz < 0)
        dz = -dz;
    if (dx > icon->radius || dz > icon->radius)
        return 0;
    return 1;
}
