#include "global.h"
#include "ui/save/save_menu.h"
void LinkScorePanel_SetNumber(struct SaveMenu *, s32, s32);

void LinkScorePanel_UpdateNumberReveal(struct SaveMenu *menu)
{
    if (menu->skip != 0) {
        menu->skip--;
        if (menu->skip == 0) {
            LinkScorePanel_SetNumber(menu, 0, menu->remaining);
            Sound_PlaySong(0x167);
        }
    }
}
