#include "global.h"
#include "scene_hud_display.h"
#include "input.h"
void MenuDisplay_UpdateActionPrompt(void *, int);
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
s32 Inventory_GetQuantityByStorage(u32, u32);
int SceneHud_CanApplySelectedAction(s32);
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_OpenPrompt(s32, s32, s32);
u32 PauseMenu_CanUseSelectedItem(void *);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_OnItemUseConfirmationUpdate(void);
void SceneHud_ReturnToActionGrid(void *);
void SceneHud_UpdateActionSelection(struct SceneHudState *p)
{
    int result, index;
    u8 *input;
    MenuDisplay_UpdateActionPrompt(p, 4);
    input = p->actionInput;
    result = SceneInput_NavigateGridKeys(input, 240);
    if (result) {
        SceneHud_GetActionIconPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowActionPreview(p);
        Hud_SaveGridSelection(input);
        MenuDisplay_UpdateActionPrompt(p, 4);
    } else if (gPadState[1] & 1) {
        index = Inventory_GetQuantityByStorage(p->actionCategory, p->actionSelection);
        if (SceneHud_CanApplySelectedAction(index) && index != -1) {
            PauseMenu_AddSpriteFlags(p, 59, 1);
            PauseMenu_OpenPrompt(*(s32 *)((u8 *)p + 0x5c), 3, 1);
            PauseMenu_SetConfirmationIndicator(p, PauseMenu_CanUseSelectedItem(p));
            MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, result);
            PauseMenu_HidePromptIcons(p);
            Hud_SetInteractionCallback(p, PauseMenu_OnItemUseConfirmationUpdate, 1);
        } else
            Sound_PlaySong(0x192);
    } else if (gPadState[1] & 2) {
        SceneHud_ReturnToActionGrid(p);
        Sound_PlaySong(222);
    }
}
