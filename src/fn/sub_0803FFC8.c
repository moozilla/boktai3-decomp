#include "modifier_list.h"
#define ModifierMenu_ApplyPendingSource sub_0803FFC8
u32 sub_0811DD94(s32, s32);
s32 sub_0803FD64(struct ModifierMenu *);
s32 sub_0803FD8C(struct ModifierMenu *);
s32 sub_0803FE68(struct ModifierMenu *);
s32 sub_0803FE94(struct ModifierMenu *);
s32 sub_0803FED4(struct ModifierMenu *);
s32 sub_0803FF20(struct ModifierMenu *);
void sub_0803FDBC(struct ModifierMenu *);
s32 ModifierMenu_ApplyPendingSource(struct ModifierMenu *menu)
{
    s32 result = -1;
    switch (menu->mode) {
        case 0:
            if (!sub_0811DD94(menu->category, menu->index))
                break;
            if (menu->category == 0)
                result = sub_0803FD64(menu);
            else if (menu->category == 1) {
                result = sub_0803FD8C(menu);
                sub_0803FDBC(menu);
            } else if (menu->category == 2)
                result = sub_0803FE68(menu);
            else
                result = sub_0803FE94(menu);
            break;
        case 1: {
            s32 category = menu->category;
            s32 index = menu->index;
            if (!sub_0811DD94(4, category * 3 * 2 + index))
                break;
            result = sub_0803FED4(menu);
            sub_0803FDBC(menu);
            break;
        }
        case 2: {
            s32 category = menu->category;
            s32 index = menu->index;
            if (sub_0811DD94(5, category * 3 * 2 + index))
                result = sub_0803FF20(menu);
            break;
        }
    }
    return result;
}
