// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
s32 Modifier_ApplyKindZero(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 Modifier_ApplyKindSeven(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 Modifier_ApplyKindSix(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void ModifierMenu_DrawStatComparison(s32, s32, s32, s32, u32);
void ModifierMenu_DrawUnavailableStat(s32, s32, s32);
void ModifierMenu_DrawCapacity(s32, s32, s32, s32, s32);
void ModifierMenu_DrawSecondSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = Modifier_ApplyKindZero(pending, current, current->values0[2], 0);
    s32 second = Modifier_ApplyKindSeven(pending, current, current->values0[3], 0);
    s32 third = Modifier_ApplyKindSix(pending, current, current->values0[4], 0);
    s32 count = current->count;
    s32 capacity = current->capacity;
    register s32 nextFirst asm("r10");
    register s32 nextSecond asm("r9");
    register s32 nextThird asm("r8");
    s32 nextCapacity;
    if (menu->mode == 1) {
        nextFirst = Modifier_ApplyKindZero(pending, current, current->values0[2], 1);
        nextSecond = Modifier_ApplyKindSeven(pending, current, current->values0[3], menu->mode);
        nextThird = Modifier_ApplyKindSix(pending, current, current->values0[4], menu->mode);
        nextCapacity = current->capacity;
    } else {
        nextFirst = Modifier_ApplyKindZero(pending, current, pending->values0[2], menu->mode);
        nextSecond = Modifier_ApplyKindSeven(pending, current, pending->values0[3], menu->mode);
        nextThird = Modifier_ApplyKindSix(pending, current, pending->values0[4], menu->mode);
        nextCapacity = pending->capacity;
    }
    switch (menu->state) {
        case 0:
        case 2:
            ModifierMenu_DrawStatComparison(first, first, 21, 6, 2);
            ModifierMenu_DrawStatComparison(second, second, 21, 7, 3);
            ModifierMenu_DrawStatComparison(third, third, 21, 8, 4);
            ModifierMenu_DrawCapacity(count, capacity, capacity, 21, 9);
            break;
        case 1:
            if (menu->mode == 0) {
                if (sub_0811DD94(menu->category, menu->index))
                    goto validPreview;
                goto invalidPreview;
            } else if (menu->mode == 1) {
                if (!sub_0811DD94(4, menu->index + menu->category * 3 * 2))
                    goto invalidPreview;
            validPreview:
                ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 2);
                ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 3);
                ModifierMenu_DrawStatComparison(third, nextThird, 8, 15, 4);
                ModifierMenu_DrawCapacity(count, capacity, nextCapacity, 8, 16);
                break;
            invalidPreview:
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
                ModifierMenu_DrawUnavailableStat(8, 15, 3);
                ModifierMenu_DrawUnavailableStat(8, 16, 3);
            } else {
                ModifierMenu_DrawStatComparison(first, first, 8, 13, 2);
                ModifierMenu_DrawStatComparison(second, second, 8, 14, 3);
                ModifierMenu_DrawStatComparison(third, third, 8, 15, 4);
                ModifierMenu_DrawCapacity(count, capacity, capacity, 8, 16);
            }
            break;
    }
}
