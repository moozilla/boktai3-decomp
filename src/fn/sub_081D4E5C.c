#include "global.h"
s32 Bestiary_GetGroupEntryCount(s32);
s32 Bestiary_GetGroupFirstEntry(s32 n)
{
    s32 s = 0;
    s32 i;
    for (i = 0; i < n; i++)
        s += Bestiary_GetGroupEntryCount(i);
    return s;
}
