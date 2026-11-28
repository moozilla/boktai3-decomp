#include "global.h"
#include "ui/save/save_menu.h"

void LinkScorePanel_RemoveSprites(struct SaveMenu *menu)
{
    s32 i;

    for (i = 0; i < 18; i++)
        Anim_Remove(&menu->widgets[i]);
}
