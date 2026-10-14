// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
struct ModifierMenu *ModifierMenu_CommitSelection(struct ModifierMenu *menu)
{
    register struct ModifierMenu *base asm("r2") = menu;
    struct ModifierSelection *pending;
    struct ModifierSelection *current;
    pending = &base->pending;
    current = &base->current;
    current->selectedId = pending->selectedId;
    current->count = pending->count;
    current->mask = pending->mask;
    return menu;
}
