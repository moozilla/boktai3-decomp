#include "modifier_list.h"
#define ModifierMenu_DrawFourthSourceStats sub_08040610
s32 sub_0803FB40(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 sub_0803FBC0(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void sub_08040098(s32, s32, s32, s32, u32);
void sub_08040144(s32, s32, s32);
void sub_08215DF0(u32, s32, s32, u32, u32, u32);
void ModifierMenu_DrawFourthSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = sub_0803FB40(pending, current, current->valuesE[3], menu->mode);
    s32 second = sub_0803FBC0(pending, current, current->valuesE[4], menu->mode);
    s32 nextFirst = sub_0803FB40(pending, current, pending->valuesE[3], menu->mode);
    s32 nextSecond = sub_0803FBC0(pending, current, pending->valuesE[4], menu->mode);
    switch (menu->state) {
        case 0:
        case 2: {
            u32 zero, one;
            sub_08040098(first, first, 25, 4, 9);
            sub_08040098(second, second, 25, 5, 10);
            zero = 0;
            sub_08215DF0(2, 24, 4, 0x1d, zero, zero);
            one = 1;
            sub_08215DF0(2, 28, 4, 0x1d, one, zero);
            sub_08215DF0(2, 24, 5, 0x1d, zero, zero);
            sub_08215DF0(2, 28, 5, 0x1d, one, zero);
            break;
        }
        case 1: {
            u32 zero, one;
            if (sub_0811DD94(menu->category, menu->index)) {
                sub_08040098(first, nextFirst, 8, 13, 9);
                sub_08040098(second, nextSecond, 8, 14, 10);
            } else {
                sub_08040144(8, 13, 3);
                sub_08040144(8, 14, 3);
            }
            zero = 0;
            sub_08215DF0(2, 7, 13, 0x1d, zero, zero);
            one = 1;
            sub_08215DF0(2, 11, 13, 0x1d, one, zero);
            sub_08215DF0(2, 7, 14, 0x1d, zero, zero);
            sub_08215DF0(2, 11, 14, 0x1d, one, zero);
            break;
        }
    }
}
