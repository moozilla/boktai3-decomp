#include "global.h"
#include "gba/io_reg.h"
#include "runtime.h"
void Solar_OnTimerInterrupt(void);
static inline u16 maskIE(u32 ie, u32 mask)
{
    return ie | mask;
}
void Solar_InitTimer(void)
{
    u16 ie;
    gSolarGpioShadow = (gSolarGpioShadow & -3) | 1;
    gSolarMeasurementPhase = 0;
    gSolarInterruptCounter = 0;
    gSolarMeasuring = 0;
    gSolarTimerPeriod = 0x3128;
    gSolarSensorParam1 = 1;
    gSolarSensorParam0 = 255;
    gSolarSensorParam2 = 4;
    gSolarMeasurement = -1;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 0;
    *(vu16 *)0x0400010C = 0;
    *(vu16 *)0x0400010E = 0xC0;
    ((void (**)(void))gSerialInterruptState)[2] = Solar_OnTimerInterrupt;
    gSolarGpioSaved = *(vu16 *)0x080000C4;
    *(vu16 *)0x080000C6 = 7;
    *(vu16 *)0x080000C8 = 1;
    REG_IE = maskIE(ie, 0x40);
    REG_IME = 1;
}
