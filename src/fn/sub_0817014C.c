
#include "global.h"
#include "scene_hud_display.h"
int SceneHud_UpdateBinaryChoice(void *);
void SceneHud_RefreshActionIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_OnItemUseSelectionUpdate(void);
void PauseMenu_OnItemSelectionTransitionUpdate(void);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void PauseMenu_AddSpriteFlags(void *, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
s32 MessageWindow_Clear(void);
void SceneHud_BeginActionIconMotion(void *, int, int, int);
void PauseMenu_ClearItemSlot(void *, u32);
void SceneHud_ResolveActionConfirmation(struct SceneHudState *p)
{
    int r = SceneHud_UpdateBinaryChoice(p);
    u8 *selection;
    if (r == 0) {
        Sound_PlaySong(222);
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, PauseMenu_OnItemUseSelectionUpdate, 1);
        sub_0816CDB8(p->actionInput, 1, 0);
        SceneHud_GetActionIconPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowActionPreview(p);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, r);
    } else if (r == 1) {
        PauseMenu_AddSpriteFlags(p, 5, 1);
        MessageWindow_Clear();
        selection = &p->actionSelection;
        SceneHud_BeginActionIconMotion(p, 0, *selection, 36);
        PauseMenu_ClearItemSlot(p, *selection);
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, PauseMenu_OnItemSelectionTransitionUpdate, 1);
    }
}
