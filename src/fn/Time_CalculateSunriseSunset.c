#include "global.h"
struct State {
    s32 date;
    u8 padding[0x14];
    s32 a, b, c;
};
extern struct State gSunClock;
void Clock_UnpackDate(s32 *, s32 *, s32 *, s32);
u32 Time_CalculateSunriseSunsetCore(s32, s32, s32, double, double, s32);

u32 Time_CalculateSunriseSunset(s32 a, s32 b, s32 c)
{
    s32 year, month, day;
    double da, db;
    gSunClock.a = a;
    gSunClock.b = b;
    gSunClock.c = c;
    da = a * 0.0000152587890625;
    db = b * 0.0000152587890625;
    Clock_UnpackDate(&year, &month, &day, gSunClock.date);
    Time_CalculateSunriseSunsetCore(year, month, day, da, db, c);
}
