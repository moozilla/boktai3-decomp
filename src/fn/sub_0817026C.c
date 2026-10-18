
#include "global.h"
#include "scene_hud_display.h"
void SceneHud_RefreshActionIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_OnItemUseSelectionUpdate(void);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void SceneHud_UpdateActionFeedback(struct SceneHudState *p)
{
    if (p->actionTransferDelay == 0)
        Sound_PlaySong(0x10E);
    if (++p->actionTransferDelay < 12) {
        if ((p->actionTransferDelay >> 1) & 1)
            p->actionFeedbackY = 104;
        else
            p->actionFeedbackY = 104 - ((20 - p->actionTransferDelay) >> 3);
    } else if (p->actionTransferDelay < 16)
        p->actionFeedbackY = 104;
    else {
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, PauseMenu_OnItemUseSelectionUpdate, 1);
        sub_0816CDB8(p->actionInput, 1, 0);
        SceneHud_GetActionIconPosition((u8 *)p + 0x1620, p->actionCursor);
        SceneHud_ShowActionPreview(p);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
    }
}
