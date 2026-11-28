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
    for (i = 0; i < 8; i++, cannon++) {
        if (p->occupiedSlots & (1 << i))
            WindCannonManager_DestroySlot(p, cannon, i);
    }
    return gFanCannonManager = 0;
}
