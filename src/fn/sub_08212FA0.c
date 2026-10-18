#include "global.h"
s32 Script_SeekToKeyword(s32);
u32 Script_GetValue(void);
s32 CollectionBook_IsEntryCollected(u32);
s32 CollectionBook_IsEntryCollectedFromScript(void)
{
    s32 r;
    if (!Script_SeekToKeyword(0x69))
        return -1;
    r = CollectionBook_IsEntryCollected(Script_GetValue());
    if (r > 0)
        r = 1;
    return r;
}
