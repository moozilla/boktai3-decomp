#include "menus/record_menu/record_menu.h"
void CharacterModel_SetAnimationFlags(void *, s32);
void LinkLobbyPanel_HideSprites(struct RecordMenu *menu)
{
    s32 i;
    u8 *p;
    for (i = 0; i < 9; i++)
        menu->sprites[i].displayFlags |= 1;
    p = menu->panelStorage;
    i = 1;
    do {
        CharacterModel_SetAnimationFlags(p, 1);
        p += 0x850;
    } while (--i >= 0);
}
