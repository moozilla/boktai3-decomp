#include "global.h"
#include "player/player.h"

// confidence=low: tests the perpendicular cell coordinate against the terrain sidestep margin.
s32 Player_IsWithinSidestepCellMargin(struct Player *p)
{
    u32 coordinate;
    u32 offset;

    if (p->coordinateAxis == 1 || p->coordinateAxis == 5)
        coordinate = (u16)p->position.x;
    else
        coordinate = (u16)p->position.z;
    offset = 255;
    offset &= coordinate;
    offset -= 31;
    if (offset > 194)
        return 0;
    return 1;
}
