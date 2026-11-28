#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"

/* Saves the HUD control mode for Hud_RestoreMode. */
void Hud_SaveMode(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud)
        hud->previousPage = hud->controlMode;
}
