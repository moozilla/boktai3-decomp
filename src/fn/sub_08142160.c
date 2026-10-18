#include "global.h"
#include "player.h"
s32 Player_IsWithinCoordinateBand(struct Player *p)
{
    u32 coordinate;
    u32 low;
    if (p->coordinateAxis == 1 || p->coordinateAxis == 5)
        coordinate = (u16)p->position.x;
    else
        coordinate = (u16)p->position.z;
    low = 0xFF;
    low &= coordinate;
    if (low - 0x1F > 0xC2)
        return 0;
    return 1;
}
