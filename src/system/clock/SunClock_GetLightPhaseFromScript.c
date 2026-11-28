#include "system/clock/sun_clock.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, u32);
u8 SunClock_GetLightPhaseFromScript(void)
{
    u8 r = gSunClock.phase;
    u32 buf[2];
    Script_ReadReference(buf);
    Script_WriteReferenceValue(buf, 0, gSunClock.phase);
    return r;
}
