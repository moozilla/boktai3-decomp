#include "script_utility.h"
u32 Script_GetValueSafe(void);
s32 ScriptUtility_ReadIndexed(void)
{
    u8 *base;
    s32 index, size;
    if (!Script_SeekToKeyword('r'))
        return -1;
    base = (u8 *)Script_GetValueSafe();
    if (!Script_SeekToKeyword('i'))
        return -1;
    index = Script_GetValue();
    if (!Script_SeekToKeyword('s'))
        return -1;
    size = Script_GetValue();
    if (!size)
        return base[index];
    {
        s32 offset;
        if (size != 1)
            offset = index * 4;
        else
            offset = index * 2;
        return *(s16 *)((u32)base + offset);
    }
}
