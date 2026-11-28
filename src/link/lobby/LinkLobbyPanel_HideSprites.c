#include "link/lobby/record_menu.h"

void CharacterModel_SetAnimationFlags(void *, s32);

void LinkLobbyPanel_HideSprites(struct RecordMenu *menu)
{
    s32 i;

    for (i = 0; i < 9; i++)
        menu->sprites[i].displayFlags |= 1;
    for (i = 0; i < 2; i++)
        CharacterModel_SetAnimationFlags(&menu->previews[i].model, 1);
}
