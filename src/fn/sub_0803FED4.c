#include "modifier_list.h"
#define ModifierMenu_ToggleSelection sub_0803FED4
u32 sub_0803F82C(struct ModifierSelection *, s32);
void sub_0803F818(struct ModifierSelection *, s32);
void sub_0803F804(struct ModifierSelection *, s32);
s32 ModifierMenu_ToggleSelection(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 id;
    current->selectedId = pending->selectedId;
    id = pending->selectedId;
    if (sub_0803F82C(current, id))
        sub_0803F818(current, id);
    else {
        if (current->count >= current->capacity)
            return -1;
        sub_0803F804(current, id);
    }
    return 0;
}
