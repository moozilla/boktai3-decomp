#include "global.h"
u32 sub_08228DB8(void);
void SunClock_GetBaseHour(void);
void SunClock_GetBaseMinute(void);
void sub_0816589C(u32, u32);
void Hud_DrawSunClockTimeA(u32, u32);
void sub_08165870(u32, u32);
void Hud_DrawSunClockTimeB(u32, u32);
void Hud_DrawStatusDayNightIndicators(void)
{
    SunClock_GetBaseHour();
    SunClock_GetBaseMinute();
    switch (sub_08228DB8()) {
        case 0:
        case 4:
        case 5:
            sub_0816589C(0x13, 0xb);
            Hud_DrawSunClockTimeA(0x18, 0xb);
            break;
        case 1:
        case 2:
        case 3:
            sub_08165870(0x13, 0xb);
            Hud_DrawSunClockTimeB(0x18, 0xb);
            break;
    }
}
