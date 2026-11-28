#include "global.h"
#include "engine/ram.h"
#include "ui/pause/scene_hud_display.h"
void Hud_UpdateTransientStatGauge(struct SceneHudState *);

void sub_08178474(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud != NULL) {
        hud->secondaryGaugeValue = hud->secondaryGaugeMaximum = 10;
        hud->secondaryGaugeTimer = 300;
        hud->secondaryGaugeFlashTimer = 0;
        Hud_UpdateTransientStatGauge(hud);
    }
}
