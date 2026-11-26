#include "global.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, u32);
extern u8 gSunClock[];
u8 SunClock_GetLightPhaseFromScript(void)
{
    u8 r = gSunClock[0xe];
    u32 buf[2];
    Script_ReadReference(buf);
    Script_WriteReferenceValue(buf, 0, gSunClock[0xe]);
    return r;
}
