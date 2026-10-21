#define SceneItem_GetSlotItem   Inventory_GetItemId
#define SceneHud_TickIconMotion PauseMenu_UpdateSpriteMotion
#include "global.h"
#include "scene_hud_display.h"
u32 SceneHud_TickIconMotion(void *, u32);
s32 SceneItem_GetSlotItem(u32);
void SceneHud_RefreshActionIcons(void *);
void SceneHud_GetActionIconPosition(void *, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void SceneHud_UpdateActionSelection(void);
void sub_0816CDB8(void *, s32, s32);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, s32);
void InventoryPanel_DrawStatComparison(void *, s32, s32, s32, s32, s32);
void MenuDisplay_SetPaletteOverrides(void *, s32, s32, s32, s32);
void SceneHud_FinishRandomActionReturn(struct SceneHudState *s)
{
    void *input;
    if (SceneHud_TickIconMotion(s, 0)) {
        if (SceneItem_GetSlotItem(s->actionSelection) == 31)
            Sound_PlaySong(0x123);
        *(u32 *)((u8 *)s + 0x2aa8) |= 1;
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
