#include "global.h"
extern u8 gUnk_03005460[];
u8 Clock_EncodeBcd(u8);
u8 Clock_CalculateWeekday(s32, s32, s32);
void Solar_PauseMeasurement(void);
void Solar_ResumeMeasurement(void);
void Clock_WaitCycles(s32);
void Clock_BeginRtcAccess(void);
void Clock_EndRtcAccess(void);
u8 Rtc_WriteDateTime(void *);
u32 sub_08229180(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    u32 result;
    int i;
    gUnk_03005460[0] = Clock_EncodeBcd((u8)(a + 0x30));
    gUnk_03005460[1] = Clock_EncodeBcd((u8)b);
    gUnk_03005460[2] = Clock_EncodeBcd((u8)c);
    gUnk_03005460[3] = Clock_CalculateWeekday(a, b, c);
    gUnk_03005460[4] = Clock_EncodeBcd((u8)d);
    gUnk_03005460[5] = Clock_EncodeBcd((u8)e);
    gUnk_03005460[6] = Clock_EncodeBcd((u8)f);
    gUnk_03005460[7] = 0;
    result = 0;
    i = 0;
    Solar_PauseMeasurement();
    while (i <= 4) {
        u16 old;
        *(vu16 *)0x04000208 = 0;
        old = *(vu16 *)0x04000200;
        *(vu16 *)0x04000200 = 0;
        Clock_BeginRtcAccess();
        result = (u8)Rtc_WriteDateTime(gUnk_03005460);
        Clock_EndRtcAccess();
        *(vu16 *)0x04000200 = old;
        *(vu16 *)0x04000208 = 1;
        if (result == 1)
            break;
        Clock_WaitCycles(200);
        i++;
    }
    Solar_ResumeMeasurement();
    return result;
}
