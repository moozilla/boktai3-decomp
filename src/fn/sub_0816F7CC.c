#include "global.h"
#include "scene_hud_display.h"
#include "scene_inventory_panel.h"
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
void InventoryPanel_ShowStatusIcons(void *);
void Hud_SaveGridSelection(void *);
void sub_0816CDB8(void *, int, int);
void PauseMenu_ClearItemSlot(void *, u32);
void SceneHud_ShowActionPreview(void *);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_RefreshActionIcons(void *);
void SceneHud_UpdateActionSelection(void);
void SceneHud_ShowActionResultText(struct SceneHudState *s)
{
    u8 *input;
    u16 frame;
    if (!s->actionTransferDelay) {
        MessageWindow_SetMessageTable(s->actionTextScript);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 4);
        MessageWindow_ShowMessage(0x77);
    }
    frame = ++s->actionTransferDelay;
    if (frame > 89 || (frame > 15 && (gPadState[1] & 1))) {
        MessageWindow_SetBounds(0, 16, 30, 2);
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, SceneHud_UpdateActionSelection, 1);
        input = s->actionInput;
        sub_0816CDB8(input, 1, 0);
        SceneHud_GetActionIconPosition(&s->categoryWidgets[5].position, s->actionCursor);
        SceneHud_ShowActionPreview(s);
        Hud_SaveGridSelection(input);
        InventoryPanel_ShowStatusIcons(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        InventoryPanel_DrawStatComparison(s, 0, 0, 0, 1, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}
