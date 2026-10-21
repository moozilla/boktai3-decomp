// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_hud_display.h"
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
void SceneHud_FinishStatBRefill(struct SceneHudState *s)
{
    u16 frame;
    u8 *input;
    s32 seven;
    Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    frame = s->actionTransferDelay;
    seven = 7;
    asm("" : "+r"(seven));
    {
        s32 m = seven;
        m &= frame;
        if (m < seven - (frame >> 3))
            s->motionWidgets[14].flags &= ~1;
        else
            s->motionWidgets[14].flags |= 1;
    }
    if (++s->actionTransferDelay > 55) {
        PauseMenu_ClearItemSlot(s, s->actionSelection);
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
