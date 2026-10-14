#include "modifier_list.h"
u32 Modifier_TestSelection(struct ModifierSelection *, s32);
void Modifier_RemoveSelection(struct ModifierSelection *, s32);
void Modifier_AddSelection(struct ModifierSelection *, s32);
s32 ModifierMenu_ToggleSelection(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 id;
    current->selectedId = pending->selectedId;
    id = pending->selectedId;
    if (Modifier_TestSelection(current, id))
        Modifier_RemoveSelection(current, id);
    else {
        if (current->count >= current->capacity)
            return -1;
        Modifier_AddSelection(current, id);
    }
    return 0;
}
