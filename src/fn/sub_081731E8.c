#include "global.h"
#include "scene_hud_display.h"
#include "input.h"
void Sound_PlaySong(u32);
void Hud_SaveSecondaryGridSelection(void *);
void SceneHud_SetActionCursorPosition(void *, u32);
void PauseMenu_AddSpriteFlags(void *, u32, u32);
void PauseMenu_ClearSpriteFlags(void *, u32, u32);
void PauseMenu_OpenPrompt(u32, u32, u32);
void PauseMenu_SetConfirmationIndicator(void *, u32);
void MenuDisplay_SetPaletteOverrides(void *, u32, u32, u32, u32);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), u32);
void MotionSequence_ConfirmReset(void);
void SceneHud_UpdateActionGridInput(void);
void SceneHud_UpdateActionChoice(void);
void SceneHud_ConfigureActionGrid(void *, u32, u32);
void Hud_SetGridPosition(void *, u32, u32);
void SceneHud_ShowSelectionDescription(void *);
void PauseMenu_RefreshFruitAndUnlockIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, u32);
u32 SceneInput_NavigateGridKeys(void *, u32);
static inline u8 *SceneHud_GetActionInput(u8 *p)
{
    return p + 0x4384;
}
void SceneHud_UpdateSecondaryActionSelection(u8 *p)
{
    if (gPadState[1] & 1) {
        u8 *grid;
        u8 *selected;
        Sound_PlaySong(0xdd);
        grid = p + 0x4384;
        Hud_SaveSecondaryGridSelection(grid);
        SceneHud_SetActionCursorPosition(p + 0x15c0, *(selected = p + 0x4387));
        if (*selected == 39) {
            PauseMenu_AddSpriteFlags(p, 5, 1);
            PauseMenu_AddSpriteFlags(p, 59, 1);
            PauseMenu_OpenPrompt(*(u32 *)(p + 0x5c), 4, 0);
            PauseMenu_SetConfirmationIndicator(p, 1);
            MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, 0);
            PauseMenu_HidePromptIcons(p);
            Hud_SetInteractionCallback(p, MotionSequence_ConfirmReset, 1);
            return;
        }
        if (*selected == 32) {
            Hud_SetInteractionCallback(p, SceneHud_UpdateActionGridInput, 1);
            SceneHud_ConfigureActionGrid(grid, 0, 0);
            MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
            Hud_SetGridPosition(grid, 0, 0);
            SceneHud_ShowSelectionDescription(p);
        } else if (*selected == 38) {
            u8 *g2;
            PauseMenu_RefreshFruitAndUnlockIcons(p);
            *(u32 *)(p + 0x1668) |= 1;
            Hud_SetInteractionCallback(p, SceneHud_UpdateActionChoice, 1);
            g2 = SceneHud_GetActionInput(p);
            Hud_SetGridPosition(g2, 0, 0);
            MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
            SceneHud_ConfigureActionGrid(g2, 0, 0);
            SceneHud_ShowSelectionDescription(p);
        }
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        Hud_SetGridPosition(((struct SceneHudState *)p)->actionInput, 0, 0);
        SceneHud_SetActionCursorPosition(p + 0x1620, *(selected = p + 0x4387));
        {
            u32 value = *selected;
            u32 prompt = 1;
            if (value <= 7)
                prompt = 0;
            MenuDisplay_UpdateActionPrompt(p, prompt);
        }
    } else if (SceneInput_NavigateGridKeys(p + 0x4384, 0xc0)) {
        u8 *selected = p + 0x4387;
        {
            u32 value = *selected;
            u32 prompt = 1;
            if (value <= 7)
                prompt = 0;
            MenuDisplay_UpdateActionPrompt(p, prompt);
        }
        SceneHud_SetActionCursorPosition(p + 0x15c0, *selected);
    }
}
