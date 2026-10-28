#include "global.h"
#include "input.h"
s32 SceneInput_NavigateGridKeys(void *, s32);
void SceneHud_GetActionIconPosition(void *, s32);
void SceneHud_ShowActionPreview(void *);
void MenuDisplay_UpdateActionPrompt(void *, s32);
void Hud_SaveGridSelection(void *);
s32 Inventory_GetQuantityByStorage(s32, s32);
void Sound_PlaySong(s32);
void PauseMenu_AddSpriteFlags(void *, s32, s32);
void PauseMenu_OpenPrompt(u32, s32, s32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void MenuDisplay_SetPaletteOverrides(void *, s32, s32, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void SceneHud_ResolveActionConfirmation(void);
void SceneHud_ReturnToActionGrid(void *);
void PauseMenu_OnItemUseSelectionUpdate(u8 *p)
{
    u8 *q = p + 0x4384;
    if (SceneInput_NavigateGridKeys(q, 0xf0)) {
        SceneHud_GetActionIconPosition(p + 0x1620, p[0x4387]);
        SceneHud_ShowActionPreview(p);
    }
    MenuDisplay_UpdateActionPrompt(p, 4);
    if (1 & gPadState[1]) {
        Hud_SaveGridSelection(q);
        if (Inventory_GetQuantityByStorage(p[0x4860], p[0x438a]) < 0)
            Sound_PlaySong(0x192);
        else {
            PauseMenu_AddSpriteFlags(p, 0x3b, 1);
            PauseMenu_OpenPrompt(*(u32 *)(p + 0x5c), 2, 1);
            PauseMenu_SetConfirmationIndicator(p, 0);
            MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, 0);
            Hud_SetInteractionCallback(p, SceneHud_ResolveActionConfirmation, 1);
        }
    } else if (2 & gPadState[1]) {
        Sound_PlaySong(0xde);
        SceneHud_ReturnToActionGrid(p);
    }
}
