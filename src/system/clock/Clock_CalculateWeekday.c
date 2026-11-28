#include "global.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 Clock_CalculateWeekday(s32 year, s32 month, s32 day)
{
    if (month <= 2) {
        year--;
        month += 12;
    }
    return Mod(year + Div(year, 4) - Div(year, 100) + Div(year, 400) + Div(13 * month + 8, 5) + day,
               7);
}
