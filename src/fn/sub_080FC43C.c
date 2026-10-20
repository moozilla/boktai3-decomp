#include "global.h"
s32 Mod(s32, s32);
extern const s16 gUnk_08606458[];
extern const s16 gUnk_08606460[];

u8 ActorPhase_UpdateReverseFlickerTimer(u32 *flags, s32 timer, s32 low, s32 high)
{
    u32 index = 2;
    if (timer <= high) {
        index = 0;
        if (timer > low)
            index = 1;
    }
    if (Mod(timer, gUnk_08606458[index]) == 0) {
        if (*flags & 1)
            *flags &= ~1;
        else
            *flags |= 1;
    }
    if (timer > 60) {
        *flags |= 1;
        return 1;
    }
    return 0;
}

u8 ActorPhase_UpdateFlicker(u32 *flags, s32 timer, s32 low, s32 high)
{
    u32 index = 2;
    if (timer <= high) {
        index = 0;
        if (timer > low)
            index = 1;
    }
    if (Mod(timer, gUnk_08606460[index]) == 0) {
        if (*flags & 1)
            *flags &= ~1;
        else
            *flags |= 1;
    }
    if (timer > 60) {
        *flags |= 1;
        return 1;
    }
    return 0;
}
