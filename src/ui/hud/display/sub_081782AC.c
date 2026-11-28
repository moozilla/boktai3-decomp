#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"

void sub_081782AC(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud) {
        if (hud->controlMode == 1 || hud->controlMode == 2)
            hud->storedEnergyFlash = 4;
    }
}
