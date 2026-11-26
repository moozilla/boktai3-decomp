#include "global.h"
extern u8 gSunClock[];
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, int, int);
void SunClock_GetSunsetSpanFromScript(void)
{
    u32 v[2];
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[12]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[13]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[16]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gSunClock[17]);
}
