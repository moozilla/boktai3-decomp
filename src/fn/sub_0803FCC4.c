// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
void Modifier_LoadFirstSource(struct ModifierSelection *);
void Modifier_LoadSecondSource(struct ModifierSelection *, const struct ModifierSelection *);
void Modifier_LoadThirdSource(struct ModifierSelection *);
void Modifier_LoadFourthSource(struct ModifierSelection *);
void ModifierMenu_SelectPendingSource(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    switch (menu->mode) {
        case 0: {
            s32 index = menu->index;
            s32 category = menu->category;
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
            register s32 category asm("r1") = menu->category;
            s32 index = menu->index;
            category = category * 3 * 2 + index;
            pending->selectedId = category;
        secondSource:
            Modifier_LoadSecondSource(pending, &menu->current);
            break;
        }
        case 2: {
            register s32 category asm("r1") = menu->category;
            s32 index = menu->index;
            category = category * 3 * 2 + index;
            pending->secondaryId = category;
            break;
        }
    }
}
