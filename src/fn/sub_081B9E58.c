#include "global.h"
#include "input.h"
#define SelectionPanel_BeginSelection Registry2388_BeginFadeOut
#define SelectionList_Confirm         MenuCursor_SetSelectionFlag
struct SelectionList {
    u8 unknown0[0x1A4];
    u32 flags;
};
struct SelectionPanel {
    u8 unknown0[0x52C];
    s32 selected;
    u32 unknown530;
    void *entries[2];
    u8 unknown53C[0x1C];
    struct SelectionList *list;
};
void Sound_PlaySong(u32);
void Registry2388_BeginFadeOut(void *);
void MenuCursor_SetSelectionFlag(void *);
void SelectionPanel_ProcessConfirm(struct SelectionPanel *panel)
{
    if (gPadState[1] & 1) {
        struct SelectionList *list = panel->list;
        u32 mask = 4;
        u32 flags = list->flags;
        if (flags & mask)
            Sound_PlaySong(0x192);
        else if ((flags & 2) && panel->selected >= 0) {
            if (panel->entries[panel->selected])
                SelectionPanel_BeginSelection(panel);
            SelectionList_Confirm(panel->list);
        }
    }
}
