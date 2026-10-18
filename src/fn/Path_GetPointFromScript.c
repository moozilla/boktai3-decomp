#include "global.h"
struct Ref {
    u8 bytes[4];
    u16 first, index;
};
u32 Script_GetValue(void);
void Script_ReadReference(struct Ref *);
void Script_WriteReferenceValue(struct Ref *, s32, s32);
s32 Path_GetPointPositionById(s16 *, u32, u32);
s32 Path_GetPointFromScript(void)
{
    s16 vector[4];
    struct Ref ref;
    s16 *saved;
    u32 first = Script_GetValue();
    u32 second = Script_GetValue();
    saved = vector;
    if (Path_GetPointPositionById(vector, first, second) < 0)
        return -1;
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, vector[0]);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, saved[1]);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, saved[2]);
    return 0;
}
