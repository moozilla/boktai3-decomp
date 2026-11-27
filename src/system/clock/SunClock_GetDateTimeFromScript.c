#include "global.h"
struct G {
    s32 packed;
    u8 a, b, c;
};
extern struct G gSunClock;
void Clock_UnpackDate(s32 *, s32 *, s32 *, s32);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);
void SunClock_GetDateTimeFromScript(void)
{
    s32 a, b, c;
    u32 context[2];
    Clock_UnpackDate(&a, &b, &c, gSunClock.packed);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, a);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, b);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, c);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, gSunClock.a);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, gSunClock.b);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, gSunClock.c);
}
