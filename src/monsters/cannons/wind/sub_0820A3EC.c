#include "monsters/cannons/wind_cannon.h"
struct FanCannon *WindCannonManager_FindFreeSlot(struct FanCannonManager *a, s32 *out)
{
    struct FanCannon *e = a->slots;
    s32 i = 0;
    u32 m = 1;
    u32 w = a->occupiedSlots;
    do {
        if ((m << i & w) != 0) {
            i++;
            e++;
            continue;
        }
        *out = i;
        return e;
    } while (i <= 7);
    return 0;
}
