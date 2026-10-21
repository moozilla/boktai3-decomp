#define SceneHud_ClearWidgetFlags PauseMenu_ClearSpriteFlags
#define SceneHud_SetWidgetFlags   PauseMenu_AddSpriteFlags
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_RefreshActionIcons(void *);
void Hud_RestoreSecondaryGridSelection(void *);
void sub_0816CDB8(void *, s32, s32);
void SceneHud_ClearWidgetFlags(void *, s32, u32);
void SceneHud_SetWidgetFlags(void *, s32, u32);
void SceneHud_GetActionIconPosition(void *, s32);
void SceneHud_ShowActionPreview(void *);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, s32);
void sub_08166534(void *, s32, s32);
void MenuDisplay_SetPaletteOverrides(void *, s32, s32, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void PauseMenu_UpdateItemActions(void);
void SceneHud_ReturnToActionGrid(struct SceneHudState *s)
{
    void *input;
    SceneHud_RefreshActionIcons(s);
    input = s->actionInput;
    Hud_RestoreSecondaryGridSelection(input);
    if (s->mode == 1)
        sub_0816CDB8(input, 1, 0xe1);
    else
        sub_0816CDB8(input, 1, 0x61);
    SceneHud_ClearWidgetFlags(s, 4, 1);
    SceneHud_SetWidgetFlags(s, 6, 1);
    SceneHud_SetWidgetFlags(s, 5, 1);
    SceneHud_GetActionIconPosition(&s->categoryWidgets[4].position, s->actionCursor);
    SceneHud_ShowActionPreview(s);
    InventoryPanel_ShowStatusIcons(s);
    MenuDisplay_UpdateActionPrompt(s, 4);
    sub_08166534(s, 10, 6);
    MenuDisplay_SetPaletteOverrides(s, 0, 1, 0, 0);
    Hud_SetInteractionCallback(s, PauseMenu_UpdateItemActions, 0);
}
