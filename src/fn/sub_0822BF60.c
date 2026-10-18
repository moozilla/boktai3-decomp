#include "global.h"
void Script_WriteReferenceValue(u8 *, s32, u32);
/* Write saved values back through the descriptors written by Registry3C8D_CaptureVariableValues. */
s32 Registry3C8D_RestoreVariableValues(u32 *save)
{
    u8 *desc = (u8 *)(save + 1);
    u32 *value = (u32 *)((u8 *)save + 0x1004);
    u32 n = 0;
    while (n < *save) {
        s32 i = 0;
        u8 *next = desc + 8;
        for (; i < *(u16 *)(desc + 4) && n < *save; i++) {
            Script_WriteReferenceValue(desc, i, *value++);
            n++;
        }
        desc = next;
    }
    return 0;
}
