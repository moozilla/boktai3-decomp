#include "ui/pause/scene_hud_display.h"
#define SceneMenuPanel_SetCallback Hud_SetInteractionCallback
#include "global.h"
#include "engine/input.h"
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_Clear(void);
void SceneMenuPanel_SetCallback(void *, void (*)(void), s32);
void MenuTransition_UpdateSelectionFade(void);
void SceneMenuPanel_ShowTimedResult(struct SceneHudState *panel)
{
    if (panel->actionTransferDelay == 16) {
        MessageWindow_SetMessageTable(panel->actionTextScript);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 4);
        MessageWindow_ShowMessage(panel->actionTransferBonus + 112);
    }
    ++panel->actionTransferDelay;
    if (panel->actionTransferDelay > 89 ||
        (panel->actionTransferDelay > 15 && (gPadState[1] & 1))) {
        MessageWindow_Clear();
        SceneMenuPanel_SetCallback(panel, MenuTransition_UpdateSelectionFade, 1);
    }
}
