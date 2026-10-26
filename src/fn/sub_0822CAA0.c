#include "global.h"
extern u8 *gGameState;
extern u8 gUnk_03005430[];
u32 SunClock_GetPackedDate(void);
s32 SunClock_GetBaseHour(void), SunClock_GetBaseMinute(void);
s32 sub_0822CAA0(void)
{
    s32 minute;
    u32 flag;
    s32 sunHour, sunMinute;
    u8 *gs;
    *(u32 *)(gGameState + 0x4e0) = SunClock_GetPackedDate();
    *(s32 *)(gGameState + 0x4e4) = SunClock_GetBaseHour();
    minute = SunClock_GetBaseMinute();
    gs = gGameState;
    *(s32 *)(gs + 0x4e8) = minute;
    sunHour = gUnk_03005430[12];
    sunMinute = gUnk_03005430[13];
    flag = 0;
    if (*(s32 *)(gs + 0x4e4) < sunHour || (*(s32 *)(gs + 0x4e4) == sunHour && minute < sunMinute))
        flag = 1;
    *(u32 *)(gGameState + 0x4ec) = flag;
}
