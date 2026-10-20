#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
void Hud_DrawDefaultHealthMeter(struct SceneHudState *, s32, s32, s32);
void Hud_DrawDefaultEnergyMeter(struct SceneHudState *, s32, s32, s32, s32, s32);
void SceneHud_DrawSlotMeter(struct SceneHudState *, s32, u32, u32);
void Hud_UpdateTimedStatusIcon(struct SceneHudState *);
void Hud_UpdateTransientStatGauge(struct SceneHudState *);
void SceneHud_RefreshActionKeyframes(struct SceneHudState *);
void SceneHud_DrawSlotMarkers(struct SceneHudState *);
void Hud_UpdateCountdownDigits(struct SceneHudState *);
void SceneHud_RefreshGameplayDisplay(struct SceneHudState *hud)
{
    if (!hud->actionOwner)
        return;
    if (hud->controlMode == 7)
        return;
    if (hud->controlMode > 2)
        return;
    Hud_DrawDefaultHealthMeter(hud, 0, 4, 1);
    Hud_DrawDefaultEnergyMeter(hud, 0, 4, 2, hud->actionOwner->currentStatB,
                               hud->actionOwner->maximumStatB);
    SceneHud_DrawSlotMeter(hud, 0, 9, 0x12);
    Hud_UpdateTimedStatusIcon(hud);
    Hud_UpdateTransientStatGauge(hud);
    SceneHud_RefreshActionKeyframes(hud);
    SceneHud_DrawSlotMarkers(hud);
    Hud_UpdateCountdownDigits(hud);
}
