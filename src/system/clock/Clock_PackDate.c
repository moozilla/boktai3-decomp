#include "global.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
u32 Clock_PackDate(s32 year, s32 month, s32 day)
{
    s32 yearOnes, yearTens, yearHundreds, yearThousands;
    s32 monthOnes, monthTens, dayOnes, dayTens;

    yearOnes = Mod(year, 10);
    yearTens = Div(Mod(year, 100) - yearOnes, 10);
    yearHundreds = Div(Mod(year, 1000) - (yearOnes + yearTens), 100);
    yearThousands = Div(Mod(year, 10000) - (yearOnes + yearTens + yearHundreds), 1000);
    monthOnes = Mod(month, 10);
    monthTens = Div(Mod(month, 100) - monthOnes, 10);
    dayOnes = Mod(day, 10);
    dayTens = Div(Mod(day, 100) - dayOnes, 10);
    return (yearThousands << 28) | (yearHundreds << 24) | (yearTens << 20) | (yearOnes << 16) |
           (monthTens << 12) | (monthOnes << 8) | (dayTens << 4) | dayOnes;
}
