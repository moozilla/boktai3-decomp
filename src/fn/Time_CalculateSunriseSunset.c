#include "global.h"
struct State {
    s32 date;
    u8 padding[0x14];
    s32 a, b, c;
};
extern struct State gUnk_03005430;
void sub_082286E0(s32 *, s32 *, s32 *, s32);
u32 Time_CalculateSunriseSunsetCore(s32, s32, s32, double, double, s32);

u32 Time_CalculateSunriseSunset(s32 a, s32 b, s32 c)
{
    s32 year, month, day;
    double da, db;
    gUnk_03005430.a = a;
    gUnk_03005430.b = b;
    gUnk_03005430.c = c;
    da = a * 0.0000152587890625;
    db = b * 0.0000152587890625;
    sub_082286E0(&year, &month, &day, gUnk_03005430.date);
    Time_CalculateSunriseSunsetCore(year, month, day, da, db, c);
}
