#include "global.h"
#include "libm_compat.h"
double Time_CalculateModifiedJulianDate(s32, u32, s32, s32);
double Time_CalculateLunarLongitude(double);
double Time_CalculateSolarLongitude(double);

double Time_CalculateMoonAge(s32 a, u32 year, s32 month, s32 day)
{
    double start, current, difference, age;
    double rate = 12.1818;
    double period = 360.0 / 12.1818;
    s32 i;
    current = Time_CalculateModifiedJulianDate(a, year, month, day) + 0.5;
    start = current;
    for (i = 1; i <= 6; i++) {
        difference = Time_CalculateLunarLongitude(current) - Time_CalculateSolarLongitude(current);
        if (difference >= 0.0 && difference <= 0.1)
            break;
        if (difference < 0.0 && difference >= -0.1)
            break;
        current -= difference / rate;
    }
    age = start - current;
    if (age < 0.0 || age > period) {
        if (age < 0.0) {
            current -= period;
            if (age < -period)
                current -= period;
        } else
            current += period;
        for (i = 1; i <= 4; i++) {
            difference = Time_CalculateLunarLongitude(current) - Time_CalculateSolarLongitude(current);
            if (difference >= 0.0 && difference <= 0.1)
                break;
            if (difference < 0.0 && difference >= -0.1)
                break;
            current -= difference / rate;
        }
    }
    age = start - current;
    return age;
}
