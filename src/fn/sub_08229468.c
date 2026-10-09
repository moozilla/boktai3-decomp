#include "global.h"

double sub_08229468(s32 unused, u32 year, s32 month, s32 day)
{
    u32 yearDays, monthDays, y;
    s32 m;
    double result;
    if (month <= 2) {
        y = year - 1;
        m = month + 12;
    } else {
        y = year;
        m = month;
    }
    yearDays = (u32)(365.25 * y);
    yearDays += y / 400;
    yearDays -= y / 100;
    result = yearDays;
    monthDays = (u32)(30.59 * (u32)(m - 2));
    result = result + (double)monthDays + day - 678912.0;
    return result;
}
