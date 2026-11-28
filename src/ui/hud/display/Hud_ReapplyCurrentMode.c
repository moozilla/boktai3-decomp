#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_SetMode(u32);
/* Re-applies the current HUD control mode. */
void Hud_ReapplyCurrentMode(void)
{
    if (gFieldHud)
        Hud_SetMode(((struct SceneHudState *)gFieldHud)->controlMode);
}
