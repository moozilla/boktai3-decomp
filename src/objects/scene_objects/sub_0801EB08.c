#include "global.h"
u8 *ResourceRegistry_GetResource(int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, int, int);
void SceneObjects_ScriptReadPosition(void)
{
    u8 *p = ResourceRegistry_GetResource(0x85e6);
    int id, i, count, index;
    u32 output[2];
    if (!p)
        return;
    if (!Script_SeekToKeyword(0x6e))
        return;
    id = Script_GetValue();
    i = 0;
    count = 0;
    index = 0;
    for (; i < *(u16 *)(p + 0x1a); i++) {
        u8 *s = *(u8 **)(p + 0x42c) + i * 140;
        if (*(u16 *)(s + 0x34) == id) {
            count++;
            index = i;
        }
    }
    if (!count)
        return;
    if (count > 1)
        return;
    if (!Script_SeekToKeyword(0x70))
        return;
    Script_ReadReference(output);
    Script_WriteReferenceValue(output, 0, *(s16 *)(*(u8 **)(p + 0x42c) + index * 140 + 0x1c));
    Script_ReadReference(output);
    Script_WriteReferenceValue(output, 0, *(s16 *)(*(u8 **)(p + 0x42c) + index * 140 + 0x1e));
    Script_ReadReference(output);
    Script_WriteReferenceValue(output, 0, *(s16 *)(*(u8 **)(p + 0x42c) + index * 140 + 0x20));
}
