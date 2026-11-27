#include "ui/pause/scene_hud_display.h"
#define SceneMenuPanel_UpdateMotion           PauseMenu_UpdateSpriteMotion
#define SceneMenuPanel_StartPaletteTransition PauseMenu_StartPaletteTransition
#define SceneMenuPanel_SetCallback            Hud_SetInteractionCallback
#include "global.h"
u32 SceneMenuPanel_UpdateMotion(struct SceneHudState *, u32);
u32 PauseMenu_GetPlayerStatusValue(struct SceneHudState *);
void SceneMenuPanel_StartPaletteTransition(struct SceneHudState *, u16, u32, s32);
void Sound_PlaySong(u32);
void SceneMenuPanel_SetCallback(struct SceneHudState *, void (*)(void), s32);
void MenuTransition_UpdateWidgetFlicker(void);
void MenuTransition_AnimateMetersToTarget(void);
void SceneMenuPanel_FinishResultMotion(struct SceneHudState *panel)
{
    if (SceneMenuPanel_UpdateMotion(panel, 0)) {
        if (++panel->actionTransferDelay > 7) {
            panel->motionWidgets[0].flags |= 1;
            SceneMenuPanel_StartPaletteTransition(panel, 366, PauseMenu_GetPlayerStatusValue(panel),
                                                  16);
            Sound_PlaySong(269);
            if (panel->actionTransferBonus == 0)
                SceneMenuPanel_SetCallback(panel, MenuTransition_UpdateWidgetFlicker, 1);
            else
                SceneMenuPanel_SetCallback(panel, MenuTransition_AnimateMetersToTarget, 1);
        }
    }
}
