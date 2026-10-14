#include "modifier_list.h"
#define ModifierMenu_DrawFirstSourceStats sub_080401E8
s32 sub_0803FB40(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 sub_0803FB98(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void sub_08040098(s32, s32, s32, s32, u32);
void sub_08040144(s32, s32, s32);
void ModifierMenu_DrawFirstSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = sub_0803FB40(pending, current, current->values0[0], 0);
    s32 second = sub_0803FB98(pending, current, current->values0[1], 0);
    s32 nextFirst = sub_0803FB40(pending, current, pending->values0[0], menu->mode);
    s32 nextSecond = sub_0803FB98(pending, current, pending->values0[1], menu->mode);
    switch (menu->state) {
        case 0:
        case 2:
            sub_08040098(first, first, 21, 4, 0);
            sub_08040098(second, second, 21, 5, 1);
            break;
        case 1:
            if (sub_0811DD94(menu->category, menu->index)) {
                sub_08040098(first, nextFirst, 8, 13, 0);
                sub_08040098(second, nextSecond, 8, 14, 1);
            } else {
                sub_08040144(8, 13, 3);
                sub_08040144(8, 14, 3);
            }
            break;
    }
}
