#include "global.h"
#define SelectionList_FindEntryAtCursor sub_081BAD98
struct SelectionListEntry {
    s16 left, top, right, bottom;
    u16 unknown8, flags;
    u32 unknownC;
};
struct SelectionList {
    u8 unknown0[0x38];
    s16 cursorX, cursorY;
    u8 unknown3C[0x64];
    s16 selected, count;
    u32 unknownA4;
    struct SelectionListEntry entries[1];
};
s32 sub_081BAD98(struct SelectionList *list)
{
    s16 *cursor = &list->cursorX;
    s32 i = 0;
    if (i < list->count) {
    u32 mask = 1;
    s32 count = list->count;
    struct SelectionListEntry *entry = list->entries;
    do {
        if (!(entry->flags & mask) && cursor[0] >= entry->left && cursor[0] <= entry->right &&
            cursor[1] >= entry->top && cursor[1] <= entry->bottom)
            return i;
        ++entry;
        ++i;
    } while (i < count);
    }
    return -1;
}
