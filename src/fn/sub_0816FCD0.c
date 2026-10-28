#include "global.h"
#include "input.h"

/* tentative: action-grid input handler for a pause-menu item screen */

void sub_0816618C(void *, s32, s32);
void Sound_PlaySong(s32);
void Hud_SaveSecondaryGridSelection(void *);
void SceneHud_GetActionIconPosition(void *, s32);
void MenuDisplay_UpdateActionPrompt(void *, s32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void PauseMenu_AddSpriteFlags(void *, s32, s32);
void PauseMenu_OpenPrompt(s32, s32, s32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void MenuDisplay_SetPaletteOverrides(void *, s32, s32, s32, s32);
void SceneHud_RefreshActionIcons(void *);
void sub_0816CDB8(void *, s32, s32);
void PauseMenu_ResetItemCursor(void *);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
void InventoryPanel_ShowStatusIcons(void *);
void InventoryPanel_DrawStatComparison(void *, s32, s32, s32, s32, s32);
void PauseMenu_ClearSpriteFlags(void *, s32, s32);
s32 SceneInput_NavigateGridKeys(void *, s32);
void PauseMenu_OnSortConfirmationUpdate(void);
void SceneHud_UpdateActionSelection(void);
void PauseMenu_OnSwapSelectionUpdate(void);
void PauseMenu_OnItemUseSelectionUpdate(void);
static inline void PauseMenu_BeginActionSelection(u8 *p)
{
    u8 *g;
    SceneHud_RefreshActionIcons(p);
    Hud_SetInteractionCallback(p, SceneHud_UpdateActionSelection, 1);
    g = p + 0x4384;
    sub_0816CDB8(g, 1, 0);
    PauseMenu_ResetItemCursor(p);
    SceneHud_GetActionIconPosition(p + 0x1620, p[0x4387]);
    SceneHud_ShowActionPreview(p);
    Hud_SaveGridSelection(g);
    InventoryPanel_ShowStatusIcons(p);
    MenuDisplay_UpdateActionPrompt(p, 4);
    InventoryPanel_DrawStatComparison(p, 0, 0, 0, 1, 0);
    MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
}
static inline void PauseMenu_BeginSwapSelection(u8 *p)
{
    SceneHud_RefreshActionIcons(p);
    Hud_SetInteractionCallback(p, PauseMenu_OnSwapSelectionUpdate, 1);
    sub_0816CDB8(p + 0x4384, 1, 0);
    PauseMenu_ResetItemCursor(p);
    SceneHud_GetActionIconPosition(p + 0x1620, p[0x4387]);
    SceneHud_ShowActionPreview(p);
    MenuDisplay_UpdateActionPrompt(p, 4);
    PauseMenu_AddSpriteFlags(p, 6, 1);
    MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
}
static inline void PauseMenu_BeginItemUseSelection(u8 *p)
{
    if (p[0x1414] == 1) {
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, PauseMenu_OnItemUseSelectionUpdate, 1);
        sub_0816CDB8(p + 0x4384, 1, 0);
        PauseMenu_ResetItemCursor(p);
        SceneHud_GetActionIconPosition(p + 0x1620, p[0x4387]);
        SceneHud_ShowActionPreview(p);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
    }
}
static inline void PauseMenu_BeginSortConfirmation(u8 *p)
{
    PauseMenu_AddSpriteFlags(p, 5, 1);
    PauseMenu_AddSpriteFlags(p, 0x3b, 1);
    PauseMenu_OpenPrompt(*(s32 *)(p + 0x5c), 4, 0);
    PauseMenu_SetConfirmationIndicator(p, 1);
    PauseMenu_HidePromptIcons(p);
    Hud_SetInteractionCallback(p, PauseMenu_OnSortConfirmationUpdate, 1);
    MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, 0);
}
void PauseMenu_UpdateItemActions(u8 *p)
{
    sub_0816618C(p, 8, 32);
    p[0xa58] = 0;
    MenuDisplay_UpdateActionPrompt(p, 4);
    if (p[0x4387] == 0x27)
        SceneHud_SetPairedIcons(p, 1, 5);
    else
        SceneHud_SetPairedIcons(p, 1, -1);
    if (1 & gPadState[1]) {
        u8 *q;
        Sound_PlaySong(0xdd);
        Hud_SaveSecondaryGridSelection(p + 0x4384);
        {
            u8 *a = p + 0x15c0;
            q = p + 0x4387;
            SceneHud_GetActionIconPosition(a, *q);
        }
        if (*q == 0x27) {
            PauseMenu_BeginSortConfirmation(p);
            return;
        }
        if (*q == 0x20)
            PauseMenu_BeginActionSelection(p);
        else if (*q == 0x26)
            PauseMenu_BeginSwapSelection(p);
        else if (*q == 0x28)
            PauseMenu_BeginItemUseSelection(p);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        PauseMenu_ResetItemCursor(p);
        SceneHud_GetActionIconPosition(p + 0x1620, p[0x4387]);
    } else if (SceneInput_NavigateGridKeys(p + 0x4384, 0xc0)) {
        SceneHud_GetActionIconPosition(p + 0x15c0, p[0x4387]);
        SceneHud_ShowActionPreview(p);
    }
}
