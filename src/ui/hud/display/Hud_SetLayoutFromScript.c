#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void SceneHud_ReadPageVisibility(struct SceneHudState *);
/* Script: applies the -o element visibility list (SceneHud_ReadPageVisibility)
   unless the HUD is still initializing. */
void Hud_SetLayoutFromScript(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud && !hud->initializationState)
        SceneHud_ReadPageVisibility(hud);
}
