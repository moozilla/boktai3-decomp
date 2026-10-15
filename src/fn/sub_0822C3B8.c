#include "global.h"
struct Context {
    u8 pad[0x4f0];
    u32 date;
    s32 hour, minute, second;
};
extern struct Context *gUnk_02000710;
u32 sub_08228D7C(void);
s32 SunClock_GetBaseHour(void), SunClock_GetBaseMinute(void), sub_08228DA0(void);
void sub_0822C3B8(void)
{
    gUnk_02000710->date = sub_08228D7C();
    gUnk_02000710->hour = SunClock_GetBaseHour();
    gUnk_02000710->minute = SunClock_GetBaseMinute();
    gUnk_02000710->second = sub_08228DA0();
}
