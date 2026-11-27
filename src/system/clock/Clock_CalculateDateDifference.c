#include "global.h"
s32 Div(s32, s32);
s32 Clock_DaysInMonth(s32, s32);
static inline s32 yearDays(s32 year)
{
    u16 yearCycle = 100;
    s32 leapYears = Div(year, 4);
    s32 centuries = Div(year, yearCycle);
    s32 leapCenturies;

    yearCycle = 400;
    leapCenturies = Div(year, yearCycle);
    return 365 * year + leapYears - centuries + leapCenturies;
}
s32 Clock_CalculateDateDifference(s32 year, s32 month, s32 day, s32 otherYear, s32 otherMonth,
                                  s32 otherDay)
{
    s32 total = yearDays(year);
    s32 otherTotal;
    s32 i;
    i = 1;
    while (i < month) {
        total += Clock_DaysInMonth(year, i);
        i++;
    }
    total += day;
    otherTotal = yearDays(otherYear);
    i = 1;
    while (i < otherMonth) {
        otherTotal += Clock_DaysInMonth(otherYear, i);
        i++;
    }
    otherTotal += otherDay;
    return total - otherTotal;
}
