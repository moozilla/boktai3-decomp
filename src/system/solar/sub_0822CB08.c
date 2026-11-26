#include "global.h"
extern u8 *gGameState;
extern u8 gSunClock[];
u32 SunClock_GetPackedDate(void);
s32 SunClock_GetBaseHour(void), SunClock_GetBaseMinute(void);
void Clock_UnpackDate(s32 *, s32 *, s32 *, s32);
u32 Clock_CalculateDateDifference(s32, s32, s32, s32, s32, s32);
u32 Solar_HasExposureCooldownElapsed(void);
s32 sub_0822CAA0(void);
#define GS(type, offset) (*(type *)(gGameState + (offset)))

void sub_0822CB08(void)
{
    u32 date = SunClock_GetPackedDate();
    s32 hour = SunClock_GetBaseHour();
    s32 minute = SunClock_GetBaseMinute();
    s32 sunHour = gSunClock[12];
    s32 sunMinute = gSunClock[13];
    s32 a, b, c, d, e, f;
    u32 diff;
    u32 reset;

    Clock_UnpackDate(&a, &b, &c, date);
    Clock_UnpackDate(&d, &e, &f, GS(u32, 0x4e0));
    diff = Clock_CalculateDateDifference(a, b, c, d, e, f);
    reset = 0;
    if (diff > 1)
        reset = 1;
    else if (diff == 1) {
        if (GS(u32, 0x4ec) || hour > sunHour || (hour == sunHour && minute >= sunMinute))
            reset = 1;
    } else {
        u32 before, after;
        before = 0;
        if (GS(s32, 0x4e4) < sunHour)
            before = 1;
        else if (GS(s32, 0x4e4) == sunHour && GS(s32, 0x4e8) < sunMinute)
            before = 1;
        after = 0;
        if (hour > sunHour || (hour == sunHour && minute >= sunMinute))
            after = 1;
        if (before && after)
            reset = 1;
    }
    if (reset) {
        GS(u16, 0x500) = 0;
        GS(u16, 0x502) = 0;
        GS(s32, 0x504) = 0;
        GS(s32, 0x508) = 0;
        GS(u16, 0x610) = 0;
        GS(u16, 0x612) = 0;
    }
    if (GS(s32, 0x504) > 29999 && Solar_HasExposureCooldownElapsed()) {
        GS(s32, 0x504) = 0;
        GS(u16, 0x502) = 0;
    }
    sub_0822CAA0();
}
