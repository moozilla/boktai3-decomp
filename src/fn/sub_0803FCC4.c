// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_SelectPendingSource sub_0803FCC4
void sub_0803FC10(struct ModifierSelection *);
void sub_0803FC34(struct ModifierSelection *, struct ModifierSelection *);
void sub_0803FC60(struct ModifierSelection *);
void sub_0803FC8C(struct ModifierSelection *);
void ModifierMenu_SelectPendingSource(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    switch (menu->mode) {
        case 0: {
            s32 index = menu->index;
            s32 category = menu->category;
            if (category == 0) {
                pending->sourceIds[0] = index;
                sub_0803FC10(pending);
            } else if (category == 1) {
                pending->sourceIds[1] = index;
                goto secondSource;
            } else if (category == 2) {
                pending->sourceIds[2] = index;
                sub_0803FC60(pending);
            } else {
                pending->sourceIds[3] = index;
                sub_0803FC8C(pending);
            }
            break;
        }
        case 1: {
            register s32 category asm("r1") = menu->category;
            s32 index = menu->index;
            category = category * 3 * 2 + index;
            pending->selectedId = category;
        secondSource:
            sub_0803FC34(pending, &menu->current);
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
