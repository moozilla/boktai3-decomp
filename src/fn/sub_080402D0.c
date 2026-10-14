// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_DrawSecondSourceStats sub_080402D0
s32 sub_0803FAF0(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 sub_0803FB18(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 sub_0803FAC8(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void sub_08040098(s32, s32, s32, s32, u32);
void sub_08040144(s32, s32, s32);
void sub_08040170(s32, s32, s32, s32, s32);
void ModifierMenu_DrawSecondSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = sub_0803FAF0(pending, current, current->values0[2], 0);
    s32 second = sub_0803FB18(pending, current, current->values0[3], 0);
    s32 third = sub_0803FAC8(pending, current, current->values0[4], 0);
    s32 count = current->count;
    s32 capacity = current->capacity;
    register s32 nextFirst asm("r10");
    register s32 nextSecond asm("r9");
    register s32 nextThird asm("r8");
    s32 nextCapacity;
    if (menu->mode == 1) {
        nextFirst = sub_0803FAF0(pending, current, current->values0[2], 1);
        nextSecond = sub_0803FB18(pending, current, current->values0[3], menu->mode);
        nextThird = sub_0803FAC8(pending, current, current->values0[4], menu->mode);
        nextCapacity = current->capacity;
    } else {
        nextFirst = sub_0803FAF0(pending, current, pending->values0[2], menu->mode);
        nextSecond = sub_0803FB18(pending, current, pending->values0[3], menu->mode);
        nextThird = sub_0803FAC8(pending, current, pending->values0[4], menu->mode);
        nextCapacity = pending->capacity;
    }
    switch (menu->state) {
        case 0:
        case 2:
            sub_08040098(first, first, 21, 6, 2);
            sub_08040098(second, second, 21, 7, 3);
            sub_08040098(third, third, 21, 8, 4);
            sub_08040170(count, capacity, capacity, 21, 9);
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
                sub_08040098(first, nextFirst, 8, 13, 2);
                sub_08040098(second, nextSecond, 8, 14, 3);
                sub_08040098(third, nextThird, 8, 15, 4);
                sub_08040170(count, capacity, nextCapacity, 8, 16);
                break;
            invalidPreview:
                sub_08040144(8, 13, 3);
                sub_08040144(8, 14, 3);
                sub_08040144(8, 15, 3);
                sub_08040144(8, 16, 3);
            } else {
                sub_08040098(first, first, 8, 13, 2);
                sub_08040098(second, second, 8, 14, 3);
                sub_08040098(third, third, 8, 15, 4);
                sub_08040170(count, capacity, capacity, 8, 16);
            }
            break;
    }
}
