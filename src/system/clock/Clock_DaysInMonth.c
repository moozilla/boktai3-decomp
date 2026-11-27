#include "global.h"
s32 Mod(s32, s32);
/* Days in `month` (1-12) of `year`, with the Gregorian leap-year rule. */
s32 Clock_DaysInMonth(s32 year, s32 month)
{
    switch (month) {
        case 2:
            if ((Mod(year, 4) == 0 && Mod(year, 100) != 0) || Mod(year, 400) == 0)
                return 29;
            return 28;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        default:
            return 31;
    }
}
