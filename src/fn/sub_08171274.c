#include "global.h"
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_OpenPrompt(void *, int, int);
void PauseMenu_SetConfirmationIndicator(void *, int);
void PauseMenu_HidePromptIcons(void *);
void PauseInventory_UpdateConsumableSortConfirmation(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_081663A0(void *, int, int, int, int);
void sub_08166B7C(void *, int);
void PauseInventory_BeginConsumableSortConfirmation(u8 *p)
{
    PauseMenu_AddSpriteFlags(p, 5, 1);
    PauseMenu_AddSpriteFlags(p, 0x3b, 1);
    PauseMenu_OpenPrompt(*(void **)(p + 0x5c), 4, 0);
    PauseMenu_SetConfirmationIndicator(p, 1);
    PauseMenu_HidePromptIcons(p);
    Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableSortConfirmation, 1);
    sub_081663A0(p, 0, 0, 1, 0);
    sub_08166B7C(p, 3);
}
