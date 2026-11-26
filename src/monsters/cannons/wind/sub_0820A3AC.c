#include "monsters/cannons/wind_cannon.h"
#include "engine/ram.h"
void WindCannonManager_InitSlot(struct FanCannonManager *, struct FanCannon *, s32);
u32 WindCannonManager_Init(struct FanCannonManager *a)
{
    struct FanCannon *e;
    s32 i;
    gFanCannonManager = a;
    a->occupiedSlots = 0;
    e = a->slots;
    i = 0;
    do {
        WindCannonManager_InitSlot(a, e, i);
        i++;
        e++;
    } while (i <= 7);
    gForceDirectionMode = 0;
    return 0;
}
