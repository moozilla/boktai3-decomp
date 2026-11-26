#include "engine/engine_pause.h"
#include "monsters/cannons/wind_cannon.h"
#include "engine/engine.h"
#include "engine/ram.h"

#include "player/player.h"
void WindCannon_Update(struct FanCannonManager *, struct FanCannon *, s32);
void WindCannon_UpdateActive(void);
void Sound_StopSong(u32);
u32 WindCannonManager_Update(struct FanCannonManager *p)
{
    u32 active = 0;
    struct FanCannon *slot;
    s32 i;
    if (Engine_PauseBits(0x400))
        return 0;
    {
        u32 pauseMask = 4;
        if (gPlayerSlotStatus[0] & pauseMask)
            return 0;
    }
    gPlayerBackgroundState = 0;
    gPlayerBackgroundMode = 0;
    for (slot = p->slots, i = 0; i < 8; i++, slot++) {
        u32 bit = 1 << i;
        if (p->occupiedSlots & bit) {
            WindCannon_Update(p, slot, i);
            if (slot->update == WindCannon_UpdateActive)
                active |= bit;
        }
    }
    if (active == 0)
        Sound_StopSong(0x519);
    return 0;
}
