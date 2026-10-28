#include "global.h"
#include "runtime.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, s32, s32);
void SaveData_WritePlayTimeToScript(void)
{
    u32 context[2];
    s32 q = Div(*((s32 *)gSessionState), 60);
    s32 r;
    Mod(q, 60);
    q = Div(q, 60);
    r = Mod(q, 60);
    q = Div(q, 60);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, 0);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, q);
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, r);
}
