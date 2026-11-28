#include "global.h"
#include "engine/engine.h"
#include "engine/runtime.h"
#include "engine/input.h"
#include "scene/areas/area_effect_link.h"
#include "system/solar/solar_measurement_task.h"

u16 sub_0822C8D8(struct SolarMeasurementTask *task)
{
    u16 *keys;
    if (gEngineFlags & 0x800) {
        s32 i, k;
        if (gLinkSession == NULL) {
            k = 1;
            i = -k;
        } else
            i = gLinkSession->localPlayer;
        keys = &gPadState[i * 4];
    } else
        keys = gPadState;
    if (keys[0] & 1) {
        if (keys[0] & 0x200) {
            if (!(++*(u16 *)&task->debugRepeat & 1) && gSolarSimulatedIntensity < 0x8c)
                gSolarSimulatedIntensity++;
        } else if (keys[0] & 0x100) {
            if (!(++*(u16 *)&task->debugRepeat & 1) && gSolarSimulatedIntensity != 0)
                gSolarSimulatedIntensity--;
        }
    }
    return gSolarSimulatedIntensity;
}
