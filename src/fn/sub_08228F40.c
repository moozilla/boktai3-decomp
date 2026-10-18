#include "global.h"
s32 Script_GetValue(void);
s32 Clock_CalculateDateDifference(s32, s32, s32, s32, s32, s32);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);
void Clock_GetDateDifferenceFromScript(void)
{
    s32 a, b, c, d, e, f, result;
    u32 context[2];
    a = Script_GetValue();
    b = Script_GetValue();
    c = Script_GetValue();
    d = Script_GetValue();
    e = Script_GetValue();
    f = Script_GetValue();
    result = Clock_CalculateDateDifference(a, b, c, d, e, f);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, result);
}
