#include "global.h"
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_OpenPrompt(void *, int, int);
void PauseMenu_SetConfirmationIndicator(void *, int);
void PauseMenu_HidePromptIcons(void *);
void PauseInventory_UpdateConsumableSortConfirmation(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void PauseInventory_BeginConsumableSortConfirmation(u8 *p)
{
    PauseMenu_AddSpriteFlags(p, 5, 1);
    PauseMenu_AddSpriteFlags(p, 0x3b, 1);
    PauseMenu_OpenPrompt(*(void **)(p + 0x5c), 4, 0);
    PauseMenu_SetConfirmationIndicator(p, 1);
    PauseMenu_HidePromptIcons(p);
    Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableSortConfirmation, 1);
    MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, 0);
    MenuDisplay_UpdateActionPrompt(p, 3);
}
