#include "monsters/cannons/wind_cannon.h"
#include "engine/ram.h"
#define WindCannonManager_RemoveCannon WindCannonManager_DestroySlot
void Sound_StopSong(u32);
void WindCannonManager_DestroySlot(struct FanCannonManager *, struct FanCannon *, u32);
u32 WindCannonManager_Destroy(struct FanCannonManager *p)
{
    struct FanCannon *cannon;
    s32 i;
    Sound_StopSong(0x519);
    cannon = p->slots;
    i = 0;
    do {
        if (p->occupiedSlots & (1 << i))
            WindCannonManager_DestroySlot(p, cannon, i);
        i++;
        cannon++;
    } while (i <= 7);
    return gFanCannonManager = 0;
}
