#include "global.h"
struct Bytes {
    u8 b0, b1, b2, b3, b4, b5, b6, b7;
};
extern struct Bytes gUnk_03005460;
void Solar_PauseMeasurement(void);
void Solar_ResumeMeasurement(void);
void Clock_WaitCycles(s32);
void Clock_BeginRtcAccess(void);
void Clock_EndRtcAccess(void);
u32 Rtc_ReadDateTime(void *);
u32 Clock_ReadValidatedRtcDateTime(void)
{
    u32 status = 0;
    s32 i;

    vu16 *ime, *ie;
    u32 zero;
    Solar_PauseMeasurement();
    i = 0;
    ime = (vu16 *)0x04000208;
    zero = 0;
    ie = (vu16 *)0x04000200;
    while (i <= 4) {
        u16 old;
        *ime = zero;
        old = *ie;
        *ie = zero;
        Clock_BeginRtcAccess();
        status = (u8)Rtc_ReadDateTime(&gUnk_03005460);
        Clock_EndRtcAccess();
        *ie = old;
        *ime = 1;
        if (status == 1)
            break;
        Clock_WaitCycles(200);
        i++;
    }
    Solar_ResumeMeasurement();
    if (status == 1) {

        if ((gUnk_03005460.b0 & 0xf0) > 0x9f || (15 & gUnk_03005460.b0) > 9) {
            status = -2;
            gUnk_03005460.b0 = 0;
        }
        if ((u8)(gUnk_03005460.b1 - 1) > 0x11 || (15 & gUnk_03005460.b1) > 9) {
            status = -3;
            gUnk_03005460.b1 = 1;
        }
        if ((u8)(gUnk_03005460.b2 - 1) > 0x30 || (15 & gUnk_03005460.b2) > 9) {
            status = -4;
            gUnk_03005460.b2 = 1;
        }
        if (gUnk_03005460.b3 == 7) {
            status = -5;
            gUnk_03005460.b3 = 0;
        }
        if (gUnk_03005460.b4 > 0x23 || (15 & gUnk_03005460.b4) > 9) {
            status = -6;
            gUnk_03005460.b4 = 0;
        }
        if (gUnk_03005460.b5 > 0x5f || (15 & gUnk_03005460.b5) > 9) {
            status = -7;
            gUnk_03005460.b5 = 0;
        }
        if (gUnk_03005460.b6 > 0x5f || (15 & gUnk_03005460.b6) > 9) {
            status = -8;
            gUnk_03005460.b6 = 0;
        }
    }
    return status;
}
