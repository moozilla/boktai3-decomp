#include "global.h"
#include "ui/save/save_menu.h"
void CharacterModel_SetAnimationFlags(void *, u32);

void LinkScorePanel_HideSprites(struct SaveMenu *menu)
{
    s32 i;

    for (i = 0; i < 18; i++)
        menu->widgets[i].flags |= 1;
    CharacterModel_SetAnimationFlags(&menu->preview.model, 1);
}
