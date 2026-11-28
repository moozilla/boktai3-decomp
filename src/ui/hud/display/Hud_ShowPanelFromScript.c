#include "global.h"
#include "engine/ram.h"
#include "ui/hud/hud.h"
#include "player/player.h"
/* Registry routine B0AA: draws the health and energy labels and meters for
   HUD element 0 (tentative). */
void Hud_ShowPanelFromScript(void)
{
    struct SceneHudState *hud = (struct SceneHudState *)gFieldHud;
    if (hud) {
        Hud_DrawHealthLabel((s32)hud, 0, 1);
        Hud_DrawHealthMeter(hud, 0, 4, 1, 1);
        Hud_DrawEnergyLabel((s32)hud, 0, 2);
        Hud_DrawEnergyMeter(hud, 0, 4, 2, hud->actionOwner->currentStatB,
                            hud->actionOwner->maximumStatB, 1);
    }
}
