#include "global.h"
extern u8 gUnk_03005460[];
s32 Clock_ReadValidatedRtcDateTime(void);
u16 Clock_DecodeBcd(u8);
void SunClock_SetDateTime(u32, u32, u32, u32, u32, u32, u32);
s32 Clock_SyncFromRtc(void)
{
    s32 result = Clock_ReadValidatedRtcDateTime();
    if (result == 1) {
        u32 year = (u16)Clock_DecodeBcd(gUnk_03005460[0]);
        u16 month, day, hour, minute, second;
        year += 2000;
        month = Clock_DecodeBcd(gUnk_03005460[1]);
        day = Clock_DecodeBcd(gUnk_03005460[2]);
        hour = Clock_DecodeBcd(gUnk_03005460[4]);
        minute = Clock_DecodeBcd(gUnk_03005460[5]);
        second = Clock_DecodeBcd(gUnk_03005460[6]);
        SunClock_SetDateTime(year, month, day, hour, minute, second, 0);
    }
    return result;
}
