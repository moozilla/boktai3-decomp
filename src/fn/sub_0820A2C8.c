#include "global.h"
#include "engine.h"
#include "ram.h"

struct FanCannon {
    u8 pad00[0xf4];
    void (*update)(void);
    u8 padF8[0x180 - 0xf8];
};
struct FanCannonManager {
    u8 pad00[0x18];
    u32 occupiedSlots;
    struct FanCannon slots[8];
};
extern u32 gUnk_02000560;
extern u16 gUnk_02000550;
extern u16 gUnk_0200053C;
void FanCannon_Update(struct FanCannonManager *, struct FanCannon *, s32);
void FanCannon_UpdateActive(void);
void Sound_StopSong(u32);
u32 FanCannonManager_Update(struct FanCannonManager *p)
{
    u32 active = 0;
    u32 mask = 0x400;
    struct FanCannon *slot;
    s32 i;
    if ((gEngineFlags | gScenePauseFlags) & mask)
        return 0;
    {
        u32 pauseMask = 4;
        if (gUnk_02000560 & pauseMask)
            return 0;
    }
    gUnk_02000550 = 0;
    gUnk_0200053C = 0;
    slot = p->slots;
    i = 0;
    do {
        u32 bit = 1 << i;
        if (p->occupiedSlots & bit) {
            FanCannon_Update(p, slot, i);
            if (slot->update == FanCannon_UpdateActive)
                active |= bit;
        }
        i++;
        slot++;
    } while (i <= 7);
    if (active == 0)
        Sound_StopSong(0x519);
    return 0;
}
