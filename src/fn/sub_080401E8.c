#include "modifier_list.h"
s32 Modifier_ApplyPositiveKindThree(struct ModifierSelection *, struct ModifierSelection *, s32,
                                    u32);
s32 Modifier_ApplyKindNine(struct ModifierSelection *, struct ModifierSelection *, s32, u32);
u32 sub_0811DD94(s32, s32);
void ModifierMenu_DrawStatComparison(s32, s32, s32, s32, u32);
void ModifierMenu_DrawUnavailableStat(s32, s32, s32);
void ModifierMenu_DrawFirstSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = Modifier_ApplyPositiveKindThree(pending, current, current->values0[0], 0);
    s32 second = Modifier_ApplyKindNine(pending, current, current->values0[1], 0);
    s32 nextFirst =
        Modifier_ApplyPositiveKindThree(pending, current, pending->values0[0], menu->mode);
    s32 nextSecond = Modifier_ApplyKindNine(pending, current, pending->values0[1], menu->mode);
    switch (menu->state) {
        case 0:
        case 2:
            ModifierMenu_DrawStatComparison(first, first, 21, 4, 0);
            ModifierMenu_DrawStatComparison(second, second, 21, 5, 1);
            break;
        case 1:
            if (sub_0811DD94(menu->choice.cursor.category, menu->choice.cursor.index)) {
                ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 0);
                ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 1);
            } else {
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
            }
            break;
    }
}
