#include "global.h"
#include "area_effect_link.h"
struct S {
    u8 pad[0x18];
    s16 x18;
};
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);

static inline s32 GetValue(void)
{
    if (!((struct S *)gLinkSession))
        return -1;
    return ((struct S *)gLinkSession)->x18;
}
s32 sub_081091AC(void)
{
    u32 context[2];
    s32 value = GetValue();
    Script_ReadReference(context);
    Script_WriteReferenceValue(context, 0, value);
    return value;
}
