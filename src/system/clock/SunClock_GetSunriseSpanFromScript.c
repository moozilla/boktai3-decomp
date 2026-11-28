#include "system/clock/sun_clock.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, int, int);
void SunClock_GetSunriseSpanFromScript(void)
{
    u32 v[2];
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.hourA);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.minuteA);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.spanAHour);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.spanAMinute);
}
