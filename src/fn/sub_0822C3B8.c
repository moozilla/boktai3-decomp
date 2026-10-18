#include "global.h"
struct Context {
    u8 pad[0x4f0];
    u32 date;
    s32 hour, minute, second;
};
extern struct Context *gUnk_02000710;
u32 SunClock_GetPackedDate(void);
s32 SunClock_GetBaseHour(void), SunClock_GetBaseMinute(void), SunClock_GetSecond(void);
void Solar_SaveExposureTimestamp(void)
{
    gUnk_02000710->date = SunClock_GetPackedDate();
    gUnk_02000710->hour = SunClock_GetBaseHour();
    gUnk_02000710->minute = SunClock_GetBaseMinute();
    gUnk_02000710->second = SunClock_GetSecond();
}
