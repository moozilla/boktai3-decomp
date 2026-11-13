#include "modifier_list.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning
void ModifierMenu_SelectPendingSource(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    switch (menu->mode) {
        case 0: {
            s32 index = menu->choice.cursor.index;
            s32 category = menu->choice.cursor.category;
            if (category == 0) {
                pending->sourceIds[0] = index;
                Modifier_LoadFirstSource(pending);
            } else if (category == 1) {
                pending->sourceIds[1] = index;
                goto secondSource;
            } else if (category == 2) {
                pending->sourceIds[2] = index;
                Modifier_LoadThirdSource(pending);
            } else {
                pending->sourceIds[3] = index;
                Modifier_LoadFourthSource(pending);
            }
            break;
        }
        case 1: {
            s32 category = menu->choice.cursor.category;
            s32 index = menu->choice.cursor.index;
            s32 scaled = category * 3 * 2;
            s32 selected = scaled + index;
            pending->selectedId = selected;
        secondSource:
            Modifier_LoadSecondSource(pending, &menu->current);
            break;
        }
        case 2: {
            s32 category = menu->choice.cursor.category;
            s32 index = menu->choice.cursor.index;
            s32 scaled = category * 3 * 2;
            s32 selected = scaled + index;
            pending->secondaryId = selected;
            break;
        }
    }
}

