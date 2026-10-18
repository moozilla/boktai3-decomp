#include "global.h"
u8 *Script_GetPc(void);
void Script_ReadReference(u8 *);
u32 Script_ReadReferenceValue(u8 *, s32);
/* Read variable descriptors (8 bytes each, element count at +4) from the
 * script and save up to 512 of their values after the count word. */
s32 Registry3C8D_CaptureVariableValues(u32 *save)
{
    u8 *desc = (u8 *)(save + 1);
    u32 *value = (u32 *)((u8 *)save + 0x1004);
    while (*save <= 0x1FF && Script_GetPc()) {
        s32 i;
        u8 *next;
        Script_ReadReference(desc);
        i = 0;
        next = desc + 8;
        for (; i < *(u16 *)(desc + 4) && *save <= 0x1FF; i++) {
            *value++ = Script_ReadReferenceValue(desc, i);
            (*save)++;
        }
        desc = next;
    }
    return 0;
}
