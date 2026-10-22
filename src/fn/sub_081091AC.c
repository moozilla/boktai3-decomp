#include "global.h"
struct S {
    u8 pad[0x18];
    s16 x18;
};
extern struct S *gUnk_020004B4;
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);

static inline s32 GetValue(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->x18;
}
s32 sub_081091AC(void)
{
    u32 context[2];
    s32 value = GetValue();
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, value);
    return value;
}
