#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void SceneHud_RebuildPrimaryOptions(struct SceneHudState *);
void PauseMenu_RefreshEquipmentSlotIcons(struct SceneHudState *);
void SceneHud_RefreshWidgetVisibility(struct SceneHudState *);
void Hud_RefreshEquipmentDisplay(void)
{
    if (gFieldHud && ((struct SceneHudState *)gFieldHud)->actionOwner) {
        SceneHud_RebuildPrimaryOptions((struct SceneHudState *)gFieldHud);
        PauseMenu_RefreshEquipmentSlotIcons((struct SceneHudState *)gFieldHud);
        if (((struct SceneHudState *)gFieldHud)->controlMode <= 2)
            SceneHud_RefreshWidgetVisibility((struct SceneHudState *)gFieldHud);
    }
}
