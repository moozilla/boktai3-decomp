#include "global.h"

// Tentative selection-list interpretation; offsets are shared by neighboring cursor code.

struct SelectionEntry {
    s16 x, y;
    u8 unknown04[8];
    u8 previous, next;
    u16 flags;
};
struct SelectionList {
    u8 unknown00[0x38];
    s16 cursorX, cursorY;
    u8 unknown3C[0x5C];
    s16 savedX, savedY;
    u8 unknown9C[4];
    s16 selected, entryCount;
    struct SelectionEntry entries[1];
};
s32 SelectionList_SelectNearestEnabledEntry(struct SelectionList *list)
{
    s32 x = list->cursorX;
    s32 y = list->cursorY;
    s32 nearest = 0;
    s32 dx = x - list->entries[0].x;
    s32 dy = y - list->entries[0].y;
    s32 distance = dx * dx + dy * dy;
    s32 i;
    for (i = 1; i < list->entryCount; i++) {
        u16 one = 1;
        if (!(list->entries[i].flags & one)) {
            s32 candidate;
            dx = x - list->entries[i].x;
            dy = y - list->entries[i].y;
            candidate = dx * dx + dy * dy;
            if (candidate < distance) {
                distance = candidate;
                nearest = i;
            }
        }
    }
    return nearest;
}
