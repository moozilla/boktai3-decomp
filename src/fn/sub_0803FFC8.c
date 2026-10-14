#include "modifier_list.h"
u32 sub_0811DD94(s32, s32);
s32 ModifierMenu_CommitFirstSource(struct ModifierMenu *);
s32 ModifierMenu_CommitSecondSource(struct ModifierMenu *);
s32 ModifierMenu_CommitThirdSource(struct ModifierMenu *);
s32 ModifierMenu_CommitFourthSource(struct ModifierMenu *);
s32 ModifierMenu_ToggleSelection(struct ModifierMenu *);
s32 ModifierMenu_CommitSecondarySource(struct ModifierMenu *);
void ModifierMenu_ReconcileSelection(struct ModifierMenu *);
s32 ModifierMenu_ApplyPendingSource(struct ModifierMenu *menu)
{
    s32 result = -1;
    switch (menu->mode) {
        case 0:
            if (!sub_0811DD94(menu->category, menu->index))
                break;
            if (menu->category == 0)
                result = ModifierMenu_CommitFirstSource(menu);
            else if (menu->category == 1) {
                result = ModifierMenu_CommitSecondSource(menu);
                ModifierMenu_ReconcileSelection(menu);
            } else if (menu->category == 2)
                result = ModifierMenu_CommitThirdSource(menu);
            else
                result = ModifierMenu_CommitFourthSource(menu);
            break;
        case 1: {
            s32 category = menu->category;
            s32 index = menu->index;
            if (!sub_0811DD94(4, category * 3 * 2 + index))
                break;
            result = ModifierMenu_ToggleSelection(menu);
            ModifierMenu_ReconcileSelection(menu);
            break;
        }
        case 2: {
            s32 category = menu->category;
            s32 index = menu->index;
            if (sub_0811DD94(5, category * 3 * 2 + index))
                result = ModifierMenu_CommitSecondarySource(menu);
            break;
        }
    }
    return result;
}
