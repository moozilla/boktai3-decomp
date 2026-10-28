#include "global.h"
#include "scene_hud_display.h"
#include "engine.h"
#include "ram.h"
extern struct Player *gUnk_02000580[];
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
    if (((struct SceneHudState *)gFieldHud)) {
        struct Player *owner = gUnk_02000580[0];
        ((struct SceneHudState *)gFieldHud)->actionOwner = owner;
        if (((struct SceneHudState *)gFieldHud)->actionOwner) {
            SceneHud_RebuildPrimaryOptions(((struct SceneHudState *)gFieldHud));
            PauseMenu_RefreshEquipmentSlotIcons(((struct SceneHudState *)gFieldHud));
            sub_08168850(((struct SceneHudState *)gFieldHud));
            SceneHud_UpdateSecondaryWidgets(((struct SceneHudState *)gFieldHud));
            if (gEngineFlags & 0x1000) {
                if (!((struct SceneHudState *)gFieldHud)->controlMode)
                    PauseMenu_RefreshStatusPanel(((struct SceneHudState *)gFieldHud));
                Hud_SetElementGroupFlags(((struct SceneHudState *)gFieldHud));
            } else if (!((struct SceneHudState *)gFieldHud)->controlMode) {
                SceneHud_RefreshWidgetVisibility(((struct SceneHudState *)gFieldHud));
                Hud_RefreshActivePanel(((struct SceneHudState *)gFieldHud));
            }
        }
    }
}
