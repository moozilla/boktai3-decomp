#include "monsters/cannons/wind_cannon.h"
struct FanCannon *WindCannonManager_FindFreeSlot(struct FanCannonManager *a, s32 *out)
{
    struct FanCannon *e = a->slots;
    s32 i;
    for (i = 0; i < 8; i++, e++) {
        if (a->occupiedSlots & (1u << i))
            continue;
        *out = i;
        return e;
    }
    return 0;
}
