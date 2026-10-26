#include "global.h"
#include "ram.h"
#define FanCannonManager_RemoveCannon FanCannonManager_DestroySlot
struct FanCannon {
    u8 unknown0[0x180];
};
struct FanCannonManager {
    u8 unknown0[0x18];
    u32 active;
    struct FanCannon cannons[8];
};
void Sound_StopSong(u32);
void FanCannonManager_DestroySlot(struct FanCannonManager *, struct FanCannon *, u32);
u32 FanCannonManager_Destroy(struct FanCannonManager *p)
{
    struct FanCannon *cannon;
    s32 i;
    Sound_StopSong(0x519);
    cannon = p->cannons;
    i = 0;
    do {
        if (p->active & (1 << i))
            FanCannonManager_DestroySlot(p, cannon, i);
        i++;
        cannon++;
    } while (i <= 7);
    return gFanCannonManager = 0;
}
