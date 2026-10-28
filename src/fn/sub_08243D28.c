#include "global.h"
#include "runtime.h"
void Solar_OnTimerInterrupt(void)
{
    u32 pins = *(vu16 *)0x080000C4;
    u32 shadow = gSolarGpioShadow;
    u32 light;
    *(vu16 *)0x080000C4 = shadow;
    *(vu16 *)0x0400010C = -gSolarTimerPeriod;
    light = 8;
    light &= pins;
    gSolarLightBit = light;
    if (gSolarMeasuring) {
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
                    gSolarMeasurement = gSolarInterruptCounter >> 1;
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
