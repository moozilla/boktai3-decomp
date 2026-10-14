#include "modifier_list.h"
#define ModifierMenu_DrawThirdSourceStats sub_080404EC
s32 sub_0803FBE8(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 sub_0803FB70(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void sub_08040098(s32, s32, s32, s32, u32);
void sub_08040144(s32, s32, s32);
void ModifierMenu_DrawThirdSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = current->valuesE[0];
    s32 second = sub_0803FBE8(pending, current, current->valuesE[1], 0);
    s32 third = sub_0803FB70(pending, current, current->valuesE[2], 0);
    s32 nextFirst = pending->valuesE[0];
    s32 nextSecond = sub_0803FBE8(pending, current, pending->valuesE[1], menu->mode);
    s32 nextThird = sub_0803FB70(pending, current, pending->valuesE[2], menu->mode);
    switch (menu->state) {
        case 0:
        case 2:
            sub_08040098(first, first, 21, 10, 6);
            sub_08040098(second, second, 21, 11, 7);
            sub_08040098(third, third, 21, 12, 8);
            break;
        case 1:
            if (sub_0811DD94(menu->category, menu->index)) {
                sub_08040098(first, nextFirst, 8, 13, 6);
                sub_08040098(second, nextSecond, 8, 14, 7);
                sub_08040098(third, nextThird, 8, 15, 8);
            } else {
                sub_08040144(8, 13, 3);
                sub_08040144(8, 14, 3);
                sub_08040144(8, 15, 3);
            }
            break;
    }
}
