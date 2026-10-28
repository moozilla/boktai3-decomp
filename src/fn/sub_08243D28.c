#include "global.h"
#include "runtime.h"
extern u32 gUnk_03006A4C, gUnk_03006A3C, gUnk_03006A38;
extern s32 gUnk_03006A48;
void Solar_OnTimerInterrupt(void)
{
    u32 pins = *(vu16 *)0x080000C4;
    u32 shadow = gSolarGpioShadow;
    u32 light;
    *(vu16 *)0x080000C4 = shadow;
    *(vu16 *)0x0400010C = -gUnk_03006A4C;
    light = 8;
    light &= pins;
    gUnk_03006A3C = light;
    if (gUnk_03006A38) {
        switch (gSolarMeasurementPhase) {
            case 0:
                if (gSolarInterruptCounter <= 9)
                    shadow |= 2;
                else
                    shadow &= -3;
                gSolarGpioShadow = shadow;
                if (gSolarInterruptCounter > 20 && light == 0) {
                    gSolarMeasurementPhase++;
                    gSolarInterruptCounter = light;
                    gSolarLastInput = light;
                }
                break;
            case 1:
                if (light != 0) {
                    gUnk_03006A48 = gSolarInterruptCounter >> 1;
                    gSolarMeasurementPhase = 2;
                }
                gSolarLastInput = light;
            case 2:
                gSolarGpioShadow ^= 1;
                if (gSolarInterruptCounter > 511) {
                    gSolarMeasurementPhase = 0;
                    gSolarInterruptCounter = 0;
                }
                break;
        }
        gSolarInterruptCounter++;
    }
}
