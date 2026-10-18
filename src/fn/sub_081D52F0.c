#include "global.h"

s32 Bestiary_GetGroupEntryCount(void);
s32 Bestiary_GetGroupFirstEntry(s32);
s32 GameState_IsEventFlagSet(s32);

u32 Bestiary_IsGroupComplete(s32 a)
{
    s32 n = Bestiary_GetGroupEntryCount();
    s32 base = Bestiary_GetGroupFirstEntry(a);
    s32 i;
    for (i = 0; i < n; i++) {
        if (GameState_IsEventFlagSet(base + i) == 0) {
            return 0;
        }
    }
    return 1;
}
