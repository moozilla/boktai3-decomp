#include "global.h"
#include "engine.h"
#include "game_state.h"
#define STATE_S16(offset) (*(s16 *)((u8 *)gGameState + (offset)))
u32 SunClock_GetSecond(void);
s32 Mod(s32, s32);
/* Script (registry 61A0): over -d days (at most 10), grows the balance at game
   state +0x7B0 by the rate at +0x598 (value * rate / 64 + 1 per day, capped at
   9999), then sets the rate to 0x41 + -R (0-13) or a random 0-12 (tentative:
   the solar bank's interest). Skipped while engine flag 0x800 is set. */
void SolarBank_ApplyInterestFromScript(void)
{
    s32 days, i, value;
    if (gEngineFlags & 0x800)
        return;
    if (Script_SeekToKeyword('d')) {
        days = Script_GetValue();
        if (days > 10)
            days = 10;
    } else
        days = 0;
    if (days <= 0)
        return;
    value = STATE_S16(0x7B0);
    if ((u32)(value - 1) <= 9997) {
        for (i = 0; i < days; i++) {
            value = ((STATE_S16(0x598) * value) >> 6) + 1;
            if (value > 9998) {
                value = 9999;
                break;
            }
        }
        STATE_S16(0x7B0) = value;
    }
    if (Script_SeekToKeyword('R')) {
        s32 rate = Script_GetValue();
        if (rate < 0)
            rate = 0;
        if (rate > 13)
            rate = 13;
        STATE_S16(0x598) = rate + 0x41;
    } else
        STATE_S16(0x598) =
            Mod(*(u32 *)((u8 *)gGameState + 0x614) ^ SunClock_GetSecond(), 13) + 0x41;
}
