#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void sub_08168850(struct SceneHudState *);
void SceneHud_UpdateSecondaryWidgets(struct SceneHudState *);
void SceneHud_RefreshWidgetVisibility(struct SceneHudState *);
void Hud_RefreshInventoryDisplay(void)
{
    if (gFieldHud && ((struct SceneHudState *)gFieldHud)->actionOwner) {
        sub_08168850((struct SceneHudState *)gFieldHud);
        SceneHud_UpdateSecondaryWidgets((struct SceneHudState *)gFieldHud);
        if (((struct SceneHudState *)gFieldHud)->controlMode <= 2)
            SceneHud_RefreshWidgetVisibility((struct SceneHudState *)gFieldHud);
    }
}
