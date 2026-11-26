#include "global.h"
extern u8 gSunClock[];
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, int, int);
void SunClock_GetSunriseSpanFromScript(void)
{
    u32 v[2];
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[10]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[11]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[18]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[19]);
}
