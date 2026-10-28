#include "global.h"
struct Bytes {
    u8 b0, b1, b2, b3, b4, b5, b6, b7;
};
extern struct Bytes gRtcDateTime;
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
        status = (u8)Rtc_ReadDateTime(&gRtcDateTime);
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

        if ((gRtcDateTime.b0 & 0xf0) > 0x9f || (15 & gRtcDateTime.b0) > 9) {
            status = -2;
            gRtcDateTime.b0 = 0;
        }
        if ((u8)(gRtcDateTime.b1 - 1) > 0x11 || (15 & gRtcDateTime.b1) > 9) {
            status = -3;
            gRtcDateTime.b1 = 1;
        }
        if ((u8)(gRtcDateTime.b2 - 1) > 0x30 || (15 & gRtcDateTime.b2) > 9) {
            status = -4;
            gRtcDateTime.b2 = 1;
        }
        if (gRtcDateTime.b3 == 7) {
            status = -5;
            gRtcDateTime.b3 = 0;
        }
        if (gRtcDateTime.b4 > 0x23 || (15 & gRtcDateTime.b4) > 9) {
            status = -6;
            gRtcDateTime.b4 = 0;
        }
        if (gRtcDateTime.b5 > 0x5f || (15 & gRtcDateTime.b5) > 9) {
            status = -7;
            gRtcDateTime.b5 = 0;
        }
        if (gRtcDateTime.b6 > 0x5f || (15 & gRtcDateTime.b6) > 9) {
            status = -8;
            gRtcDateTime.b6 = 0;
        }
    }
    return status;
}
