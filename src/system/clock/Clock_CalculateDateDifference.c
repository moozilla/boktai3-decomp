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
    for (i = 1; i < month; i++)
        total += Clock_DaysInMonth(year, i);
    total += day;
    otherTotal = yearDays(otherYear);
    for (i = 1; i < otherMonth; i++)
        otherTotal += Clock_DaysInMonth(otherYear, i);
    otherTotal += otherDay;
    return total - otherTotal;
}
