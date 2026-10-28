#include "global.h"
#include "input.h"

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
void Sound_PlaySong(u32);
s32 SelectionList_SelectNearestEnabledEntry(struct SelectionList *);
void MenuCursor_SetPositionFromSelection(struct SelectionList *);
void SelectionList_TickCursorShake(void);
void SelectionCursor_SetState(void *, void (*)(void));

s32 SelectionList_ProcessCursorInput(struct SelectionList *list)
{
    s16 *cursor = &list->savedX;
    s32 step;
    s32 old, entry;
    step = (gPadState[0] & 2) ? 2 : 1;
    if (gPadState[0] & 0x20)
        cursor[0] -= step;
    else if (gPadState[0] & 0x10)
        cursor[0] += step;
    if (gPadState[0] & 0x40)
        cursor[1] -= step;
    else if (gPadState[0] & 0x80)
        cursor[1] += step;
    if (cursor[0] < 8)
        cursor[0] = 8;
    else if (cursor[0] > 232)
        cursor[0] = 232;
    if (cursor[1] < 8)
        cursor[1] = 8;
    else if (cursor[1] > 160)
        cursor[1] = 160;
    if (gPadState[1] & 0x200) {
        Sound_PlaySong(0xDC);
        if (list->selected >= 0) {
            old = list->selected;
            entry = list->entries[old].previous;
            goto validate;
        }
        goto nearest;
    } else if (gPadState[1] & 0x100) {
        Sound_PlaySong(0xDC);
        if (list->selected >= 0) {
            old = list->selected;
            entry = list->entries[old].next;
            goto validate;
        }
        goto nearest;
    }
    goto noInput;
validate: {
    struct SelectionEntry *point = (struct SelectionEntry *)((entry << 4) + (u32)list + 0xA4);
    u16 one = 1;
    if (point->flags & one)
        entry = old;
}
    goto apply;
nearest:
    entry = SelectionList_SelectNearestEnabledEntry(list);
apply:
    list->selected = entry;
    MenuCursor_SetPositionFromSelection(list);
    SelectionCursor_SetState(list, SelectionList_TickCursorShake);
    return 1;
noInput:
    return 0;
}
