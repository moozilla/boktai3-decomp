#include "global.h"
struct Context {
    u8 pad[0x4f0];
    u32 date;
    s32 hour, minute, second;
};
extern struct Context *gUnk_02000710;
u32 SunClock_GetPackedDate(void);
s32 SunClock_GetBaseHour(void), SunClock_GetBaseMinute(void), SunClock_GetSecond(void);
void Clock_UnpackDate(s32 *, s32 *, s32 *, s32);
u32 Clock_CalculateDateDifference(s32, s32, s32, s32, s32, s32);
u32 Solar_HasExposureCooldownElapsed(void)
{
    s32 delta;
    s32 a, b, c, d, e, f;
    s32 hour, minute, second, time, old;
    if (SunClock_GetPackedDate() == gUnk_02000710->date)
        delta = 0;
    else {
        Clock_UnpackDate(&a, &b, &c, SunClock_GetPackedDate());
        Clock_UnpackDate(&d, &e, &f, gUnk_02000710->date);
        if (Clock_CalculateDateDifference(a, b, c, d, e, f) > 1)
            goto yes;
        delta = 86400;
    }
    hour = SunClock_GetBaseHour();
    minute = SunClock_GetBaseMinute();
    second = SunClock_GetSecond();
    time = (hour * 60 + minute) * 60 + second;
    old = (gUnk_02000710->hour * 60 + gUnk_02000710->minute) * 60 + gUnk_02000710->second;
    delta += time - old;
    if (delta > 179)
        goto yes;
    return 0;
yes:
    return 1;
}
