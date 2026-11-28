#include "system/clock/sun_clock.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, int, int);
void SunClock_GetSunsetSpanFromScript(void)
{
    u32 v[2];
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.hourB);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.minuteB);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.spanBHour);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock.spanBMinute);
}
