#include "modifier_list.h"
s32 Modifier_ApplyPositiveKindThree(struct ModifierSelection *, struct ModifierSelection *, s32,
                                    u32);
s32 Modifier_ApplyKindTen(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void ModifierMenu_DrawStatComparison(s32, s32, s32, s32, u32);
void ModifierMenu_DrawUnavailableStat(s32, s32, s32);
void sub_08215DF0(u32, s32, s32, u32, u32, u32);
void ModifierMenu_DrawFourthSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = Modifier_ApplyPositiveKindThree(pending, current, current->valuesE[3], menu->mode);
    s32 second = Modifier_ApplyKindTen(pending, current, current->valuesE[4], menu->mode);
    s32 nextFirst =
        Modifier_ApplyPositiveKindThree(pending, current, pending->valuesE[3], menu->mode);
    s32 nextSecond = Modifier_ApplyKindTen(pending, current, pending->valuesE[4], menu->mode);
    switch (menu->state) {
        case 0:
        case 2: {
            u32 zero, one;
            ModifierMenu_DrawStatComparison(first, first, 25, 4, 9);
            ModifierMenu_DrawStatComparison(second, second, 25, 5, 10);
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
                ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 9);
                ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 10);
            } else {
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
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
