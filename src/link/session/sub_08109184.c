#include "global.h"
s32 Script_GetValue(void);
s32 LinkSession_CountPlayerKind(s32 kind);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);

s32 sub_08109184(void)
{
    u32 context[2];
    s32 count = LinkSession_CountPlayerKind(Script_GetValue());
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, count);
    return count;
}
