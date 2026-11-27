#include "ui/pause/scene_hud_display.h"
#define SceneMenuPanel_UpdateMotion PauseMenu_UpdateSpriteMotion
#define SceneMenuPanel_SetCallback  Hud_SetInteractionCallback
#include "global.h"
u32 SceneMenuPanel_UpdateMotion(struct SceneHudState *, u32);
void PauseMenu_ApplyItemSlot(struct SceneHudState *, u32);
void Sound_PlaySong(u32);
void SceneMenuPanel_SetCallback(struct SceneHudState *, void (*)(void), s32);
void SceneHud_ShowActionResultText(void);
void SceneMenuPanel_AdvanceAfterMotion(struct SceneHudState *panel)
{
    if (SceneMenuPanel_UpdateMotion(panel, 0)) {
        if (++panel->actionTransferDelay > 7) {
            Sound_PlaySong(269);
            panel->motionWidgets[0].flags |= 1;
            PauseMenu_ApplyItemSlot(panel, panel->actionSelection);
            SceneMenuPanel_SetCallback(panel, SceneHud_ShowActionResultText, 1);
        }
    }
}
