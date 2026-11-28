#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

u32 Ratatosk_IsPositionOutsideArena(const struct ContactPosition *position)
{
    u32 outside = FALSE;

    if (position->x < 0x424)
        outside = TRUE;
    if (position->x > 0xCDA)
        outside = TRUE;
    if (position->z < 0x528)
        outside = TRUE;
    if (position->z > 0xBD6)
        outside = TRUE;
    return outside;
}
