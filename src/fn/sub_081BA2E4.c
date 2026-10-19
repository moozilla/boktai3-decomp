#define gRegistry2388SelectionEntries gUnk_0824F7E8
#include "global.h"

struct Registry2388 {
    u8 unknown0[0x528];
    u16 availableEntries;
    u8 unknown52A[0x2e];
    u32 selectionCursor;
};
struct Registry2388SelectionEntry {
    u8 unknown0[12];
    u8 linkC, linkD;
    u16 flags;
};
extern const struct Registry2388SelectionEntry gUnk_0824F7E8[9];
void *memcpy(void *, const void *, u32);
u32 MenuCursor_Create(s32, s32, s32, void *);

static inline u32 Entry_TestFlags(struct Registry2388SelectionEntry *entry, u32 mask)
{
    return entry->flags & mask;
}

void Registry2388_CreateSelectionCursor(struct Registry2388 *object, s32 selection)
{
    struct Registry2388SelectionEntry entries[9];
    s32 i;
    s32 available;
    struct Registry2388SelectionEntry *entry8, *entry6, *entry7;
    memcpy(entries, gUnk_0824F7E8, sizeof(entries));
    i = 0;
    entry8 = &entries[8];
    entry6 = &entries[6];
    entry7 = &entries[7];
    available = object->availableEntries;
    for (; i < 9; i++) {
        if ((available >> i) & 1)
            entries[i].flags = 0;
        else if ((u32)(i - 6) <= 2)
            entries[i].flags |= 3;
        else
            entries[i].flags |= 2;
    }
    if (!Entry_TestFlags(entry8, 1)) {
        entries[1].linkC = 8;
        entries[5].linkD = 8;
    }
    if (!Entry_TestFlags(entry6, 1)) {
        entries[2].linkD = 6;
        if (!Entry_TestFlags(entry7, 1))
            entries[6].linkD = 7;
        else
            entries[3].linkC = 6;
    }
    if (!Entry_TestFlags(entry7, 1)) {
        entries[3].linkC = 7;
        if (!Entry_TestFlags(entry6, 1))
            entries[7].linkC = 6;
        else
            entries[2].linkD = 7;
    }
    object->selectionCursor = MenuCursor_Create(1, selection, 9, entries);
}
