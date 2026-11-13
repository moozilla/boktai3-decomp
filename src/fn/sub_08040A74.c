#include "modifier_list.h"
s32 ModifierMenu_GetHeaderFrame(struct ModifierMenu *menu)
{
    switch (menu->mode) {
        case 0:
            return menu->choice.cursor.category + 19;
        case 1:
            return menu->choice.cursor.category + 23;
        case 2:
            return menu->choice.cursor.category + 27;
    }
}
