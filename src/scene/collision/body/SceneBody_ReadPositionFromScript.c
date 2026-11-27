#include "global.h"
struct S {
    u8 p0[12];
    s16 x, y, z;
};
s32 Script_GetValue(void);
struct S *SceneBody_FindById(u16);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);
s32 SceneBody_ReadPositionFromScript(void)
{
    u32 context[2];
    s32 result;
    struct S *s = SceneBody_FindById((u16)Script_GetValue());
    if (s) {
        Script_ReadReference(context);
        Script_WriteReferenceValue(context, 0, s->x);
        Script_ReadReference(context);
        Script_WriteReferenceValue(context, 0, s->y);
        Script_ReadReference(context);
        Script_WriteReferenceValue(context, 0, s->z);
        result = 0;
    } else {
        Script_ReadReference(context);
        Script_WriteReferenceValue(context, 0, 0);
        Script_ReadReference(context);
        Script_WriteReferenceValue(context, 0, 0);
        Script_ReadReference(context);
        Script_WriteReferenceValue(context, 0, 0);
        result = -1;
    }
    return result;
}
