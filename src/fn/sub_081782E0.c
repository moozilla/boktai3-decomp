#include "global.h"
#include "scene_hud_display.h"
#define gSceneHud gUnk_02000214
extern struct SceneHudState *gSceneHud;
extern struct Player *gUnk_02000580[];
extern u32 gUnk_030053F4;
void SceneHud_RebuildPrimaryOptions(struct SceneHudState *);
void PauseMenu_RefreshEquipmentSlotIcons(struct SceneHudState *);
void sub_08168850(struct SceneHudState *);
void SceneHud_UpdateSecondaryWidgets(struct SceneHudState *);
void PauseMenu_RefreshStatusPanel(struct SceneHudState *);
void Hud_SetElementGroupFlags(struct SceneHudState *);
void SceneHud_RefreshWidgetVisibility(struct SceneHudState *);
void Hud_RefreshActivePanel(struct SceneHudState *);
void SceneHud_RefreshControls(void)
{
    if (gSceneHud) {
        struct Player *owner = gUnk_02000580[0];
        gSceneHud->actionOwner = owner;
        if (gSceneHud->actionOwner) {
            SceneHud_RebuildPrimaryOptions(gSceneHud);
            PauseMenu_RefreshEquipmentSlotIcons(gSceneHud);
            sub_08168850(gSceneHud);
            SceneHud_UpdateSecondaryWidgets(gSceneHud);
            if (gUnk_030053F4 & 0x1000) {
                if (!gSceneHud->controlMode)
                    PauseMenu_RefreshStatusPanel(gSceneHud);
                Hud_SetElementGroupFlags(gSceneHud);
            } else if (!gSceneHud->controlMode) {
                SceneHud_RefreshWidgetVisibility(gSceneHud);
                Hud_RefreshActivePanel(gSceneHud);
            }
        }
    }
}
