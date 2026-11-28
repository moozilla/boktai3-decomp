#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_UpdateCountdownDisplay(s32 value, s32 limit, u32 effect)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud && !hud->controlMode) {
        if (value < 0)
            hud->secondaryGaugeValue = 0;
        else
            hud->secondaryGaugeValue = value;
        hud->secondaryGaugeMaximum = limit;
        hud->secondaryGaugeTimer = 300;
        if (effect)
            hud->secondaryGaugeFlashTimer = 6;
        else
            hud->secondaryGaugeFlashTimer = 0;
    }
}
