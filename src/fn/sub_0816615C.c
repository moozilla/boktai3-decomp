#include "global.h"
void SceneHud_SetPairedIcons(s32, s32, s32);
void PauseMenu_HidePromptIcons(s32 p)
{
    SceneHud_SetPairedIcons(p, -1, -1);
}
