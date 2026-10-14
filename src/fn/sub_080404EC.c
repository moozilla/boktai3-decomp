#include "modifier_list.h"
s32 Modifier_ApplyKindTwelve(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
s32 Modifier_ApplyKindEight(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void ModifierMenu_DrawStatComparison(s32, s32, s32, s32, u32);
void ModifierMenu_DrawUnavailableStat(s32, s32, s32);
void ModifierMenu_DrawThirdSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = current->valuesE[0];
    s32 second = Modifier_ApplyKindTwelve(pending, current, current->valuesE[1], 0);
    s32 third = Modifier_ApplyKindEight(pending, current, current->valuesE[2], 0);
    s32 nextFirst = pending->valuesE[0];
    s32 nextSecond = Modifier_ApplyKindTwelve(pending, current, pending->valuesE[1], menu->mode);
    s32 nextThird = Modifier_ApplyKindEight(pending, current, pending->valuesE[2], menu->mode);
    switch (menu->state) {
        case 0:
        case 2:
            ModifierMenu_DrawStatComparison(first, first, 21, 10, 6);
            ModifierMenu_DrawStatComparison(second, second, 21, 11, 7);
            ModifierMenu_DrawStatComparison(third, third, 21, 12, 8);
            break;
        case 1:
            if (sub_0811DD94(menu->category, menu->index)) {
                ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 6);
                ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 7);
                ModifierMenu_DrawStatComparison(third, nextThird, 8, 15, 8);
            } else {
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
                ModifierMenu_DrawUnavailableStat(8, 15, 3);
            }
            break;
    }
}
