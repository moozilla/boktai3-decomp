#include "system/clock/sun_clock.h"
void Clock_UnpackDate(s32 *, s32 *, s32 *, s32);
u32 Time_CalculateSunriseSunsetCore(s32, s32, s32, double, double, s32);

u32 Time_CalculateSunriseSunset(s32 latitude, s32 longitude, s32 zone)
{
    s32 year, month, day;
    double degreesA, degreesB;
    gSunClock.sunriseSunsetArgs[0] = latitude;
    gSunClock.sunriseSunsetArgs[1] = longitude;
    gSunClock.sunriseSunsetArgs[2] = zone;
    degreesA = latitude * 0.0000152587890625;
    degreesB = longitude * 0.0000152587890625;
    Clock_UnpackDate(&year, &month, &day, gSunClock.stamp);
    return Time_CalculateSunriseSunsetCore(year, month, day, degreesA, degreesB, zone);
}
