#include "ui/pause/scene_hud_display.h"
#define SceneMenuPanel_UpdateMotion           PauseMenu_UpdateSpriteMotion
#define SceneMenuPanel_StartPaletteTransition PauseMenu_StartPaletteTransition
#define SceneMenuPanel_SetCallback            Hud_SetInteractionCallback
#include "global.h"
u32 SceneMenuPanel_UpdateMotion(struct SceneHudState *, u32);
u32 PauseMenu_GetPlayerStatusValue(struct SceneHudState *);
void SceneMenuPanel_StartPaletteTransition(struct SceneHudState *, u16, u32, s32);
void PauseMenu_ApplyItemSlot(struct SceneHudState *, u32);
void Sound_PlaySong(u32);
void SceneMenuPanel_SetCallback(struct SceneHudState *, void (*)(void), s32);
void PauseMenu_OnItemUseDelayUpdate(void);
void SceneMenuPanel_FinishMotionDelay(struct SceneHudState *panel)
{
    if (SceneMenuPanel_UpdateMotion(panel, 0)) {
        if (++panel->actionTransferDelay > 7) {
            panel->motionWidgets[0].flags |= 1;
            PauseMenu_ApplyItemSlot(panel, panel->actionSelection);
            SceneMenuPanel_StartPaletteTransition(panel, 366, PauseMenu_GetPlayerStatusValue(panel),
                                                  16);
            Sound_PlaySong(269);
            SceneMenuPanel_SetCallback(panel, PauseMenu_OnItemUseDelayUpdate, 1);
        }
    }
}
