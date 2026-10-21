#include "global.h"
int SceneHud_UpdateBinaryChoice(void *);
void Sound_PlaySong(int);
void SceneHud_RefreshActionIcons(void *);
void SceneHud_UpdateActionSelection(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void MessageWindow_Clear(void);
void SceneHud_DispatchSelectedAction(void *);
void PauseMenu_OnItemUseConfirmationUpdate(u8 *p)
{
    int r = SceneHud_UpdateBinaryChoice(p);
    u8 *q, *a;
    if (r == 0) {
        Sound_PlaySong(0xde);
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionSelection, 1);
        q = p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        a = p + 0x1620;
        SceneHud_GetActionIconPosition(a, p[0x4387]);
        SceneHud_ShowActionPreview(p);
        Hud_SaveGridSelection(q);
        InventoryPanel_ShowStatusIcons(p);
        MenuDisplay_UpdateActionPrompt(p, 4);
        InventoryPanel_DrawStatComparison(p, 0, 0, 0, 1, r);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, r);
    } else if (r == 1) {
        MessageWindow_Clear();
        SceneHud_DispatchSelectedAction(p);
    }
}
