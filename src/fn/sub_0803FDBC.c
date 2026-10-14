#include "modifier_list.h"
#define ModifierMenu_ReconcileSelection sub_0803FDBC
u32 sub_0803F82C(struct ModifierSelection *, s32);
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
            if (sub_0803F82C(current, index)) {
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
        ((s16 *)gUnk_02000710->unk7A8)[index] = -1;
    for (count = 0, index = 0; index <= 31; index++) {
        if (sub_0803F82C(current, index)) {
            ((s16 *)gUnk_02000710->unk7A8)[count] = index;
            count++;
        }
    }
}
