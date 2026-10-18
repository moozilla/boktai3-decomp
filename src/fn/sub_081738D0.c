#define MotionSequence_SetCallback Hud_SetInteractionCallback
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct MotionSequence { u8 unknown0[0x5C]; void *panel; };
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_OpenPrompt(void *, s32, s32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void PauseMenu_HidePromptIcons(void *);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_UpdateInventorySortConfirmation(void);
void MotionSequence_BeginResetPrompt(struct MotionSequence *p)
{
    PauseMenu_AddSpriteFlags(p, 5, 1);
    PauseMenu_AddSpriteFlags(p, 0x3B, 1);
    PauseMenu_OpenPrompt(p->panel, 4, 0);
    PauseMenu_SetConfirmationIndicator(p, 1);
    PauseMenu_HidePromptIcons(p);
    MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, 0);
    Hud_SetInteractionCallback(p, Hud_UpdateInventorySortConfirmation, 1);
}
