#define MenuPanel_SetCallback Hud_SetInteractionCallback
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
#include "input.h"
void Sound_PlaySong(u32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void SceneCatalog_SelectDescription(u32);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void SelectionPanel_ProcessFlaggedChoice(void);
void SelectionPanel_ConfirmTransition(void *p)
{
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        PauseMenu_SetConfirmationIndicator(p, 1);
        SceneCatalog_SelectDescription(9);
        PauseMenu_HidePromptIcons(p);
        Hud_SetInteractionCallback(p, SelectionPanel_ProcessFlaggedChoice, 1);
        MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, 0);
    }
}
