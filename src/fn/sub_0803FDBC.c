#include "modifier_list.h"
u32 Modifier_TestSelection(struct ModifierSelection *, s32);
void ModifierMenu_ReconcileSelection(struct ModifierMenu *menu)
{
    struct ModifierSelection *current = &menu->current;
    s32 count, index;
    u32 mask;
    if (current->count > current->capacity) {
        count = 0;
        mask = 0;
        index = 0;
        for (;;) {
            if (index > 31)
                break;
            if (Modifier_TestSelection(current, index)) {
                mask |= 1 << index;
                count++;
                if (count >= current->capacity)
                    break;
            }
            index++;
        }
        current->count = count;
        current->mask = mask;
        if (menu->mode == 0)
            menu->changed = 1;
    }
    for (index = 0; index <= 3; index++)
        ((s16 *)gUnk_02000710->modifierIdsBytes)[index] = -1;
    for (count = 0, index = 0; index <= 31; index++) {
        if (Modifier_TestSelection(current, index)) {
            ((s16 *)gUnk_02000710->modifierIdsBytes)[count] = index;
            count++;
        }
    }
}
