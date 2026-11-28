#include "global.h"
#include "ui/save/save_menu.h"
s32 Mod(s32, s32);
void LinkMenu_DrawRandomizedFourDigitNumber(struct SaveMenu *, s32);
void LinkScorePanel_UpdateRollingNumber(struct SaveMenu *menu)
{
    if (Mod(menu->timer, 2) == 0)
        Sound_PlaySong(0x2A5);
    if (menu->skip != 0)
        LinkMenu_DrawRandomizedFourDigitNumber(menu, 0);
}
