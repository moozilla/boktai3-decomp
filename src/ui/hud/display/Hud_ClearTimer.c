#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_ClearTimer(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud) {
        hud->countdownValue = 0;
        hud->countdownDigits[0].flags |= 1;
        hud->countdownDigits[1].flags |= 1;
        hud->countdownDigits[2].flags |= 1;
        hud->countdownEnabled = 0;
        hud->actionCueInitialized = 0;
    }
}
