#include "global.h"
/* Storage menu; initializes six identity-mapped entries and option e. */
struct Storage {
    u8 pad0[0x1754];
    u8 selection;
    u8 entryCount;
    u8 pad1756[2];
    u8 entries[6];
    u8 pad175E[0x8e];
    s32 optionE;
};
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
void Storage_InitEntries(struct Storage *p)
{
    s32 i;
    s32 value;
    p->selection = 0;
    p->entryCount = 6;
    i = 0;
    do {
        p->entries[i] = i;
        i++;
    } while (i < p->entryCount);
    value = Script_SeekToKeyword(0x65);
    if (value)
        p->optionE = Script_GetValue();
    else
        p->optionE = value;
}
