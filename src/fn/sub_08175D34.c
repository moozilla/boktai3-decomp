#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
#include "input.h"
void Sound_PlaySong(u32);
void sub_0817597C(void *, int);
void SceneCatalog_SelectDescription(u32);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_UpdateConfigChoice(void);
void SelectionPanel_ConfirmSettings(void *p)
{
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        sub_0817597C(p, 1);
        SceneCatalog_SelectDescription(6);
        PauseMenu_HidePromptIcons(p);
        Hud_SetInteractionCallback(p, PauseMenu_UpdateConfigChoice, 1);
    }
}
