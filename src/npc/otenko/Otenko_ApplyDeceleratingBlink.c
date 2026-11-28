#include "npc/otenko/otenko_actor.h"

u32 Otenko_ApplyDeceleratingBlink(struct OtenkoActor *npc, s32 tick, s32 count)
{
    s32 phase;
    if (tick > (count * (count + 1)) >> 1)
        return 1;
    for (phase = 0; phase < count; phase++) {
        tick -= count - phase;
        if (tick <= 0)
            break;
    }
    if ((phase & 1) == 0) {
        u32 mask = 0x100;
        npc->displayFlags = mask | npc->displayFlags;
    }
    return 0;
}
