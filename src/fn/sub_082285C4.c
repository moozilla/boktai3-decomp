// MATCHING_CONCESSION: register-pinning
#include "global.h"
s32 Div(s32, s32);
s32 Clock_DaysInMonth(s32, s32);
static inline s32 divideYear(s32 divisor, s32 value)
{
    return Div(value, divisor);
}
static inline s32 yearDays(s32 year)
{
    s32 a = Div(year, 4), b = Div(year, 100);
    s32 c;
    register s32 k asm("r1") = 400;
    c = Div(year, k);
    return 365 * year + a - b + c;
}
s32 Clock_CalculateDateDifference(s32 year, s32 month, s32 day, s32 otherYear, s32 otherMonth, s32 otherDay)
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
