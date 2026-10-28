#include "global.h"
#include "gba/io_reg.h"
#include "runtime.h"
extern u32 gUnk_03006A38, gUnk_03006A4C;
extern u32 gUnk_03006A40, gUnk_03006A30, gUnk_03006A44, gUnk_03006A34;
extern s32 gUnk_03006A48;
extern void (*gSerialInterruptState[])(void);
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
    gUnk_03006A38 = 0;
    gUnk_03006A4C = 0x3128;
    gUnk_03006A40 = 1;
    gUnk_03006A30 = 255;
    gUnk_03006A44 = 4;
    gUnk_03006A48 = -1;
    REG_IME = 0;
    ie = REG_IE;
    REG_IE = 0;
    *(vu16 *)0x0400010C = 0;
    *(vu16 *)0x0400010E = 0xC0;
    gSerialInterruptState[2] = Solar_OnTimerInterrupt;
    gUnk_03006A34 = *(vu16 *)0x080000C4;
    *(vu16 *)0x080000C6 = 7;
    *(vu16 *)0x080000C8 = 1;
    REG_IE = maskIE(ie, 0x40);
    REG_IME = 1;
}
