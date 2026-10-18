#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#define SceneHud_RefreshGameplayDisplay sub_08178820
#define SceneHud_RefreshActionKeyframes sub_081785F4
#define SceneHud_DrawSlotMeter          sub_0817871C
void Hud_DrawDefaultHealthMeter(struct SceneHudState *, s32, s32, s32);
void Hud_DrawDefaultEnergyMeter(struct SceneHudState *, s32, s32, s32, s32, s32);
void SceneHud_DrawSlotMeter(struct SceneHudState *, s32, u32, u32);
void sub_08165020(struct SceneHudState *);
void sub_08164DC0(struct SceneHudState *);
void SceneHud_RefreshActionKeyframes(struct SceneHudState *);
void sub_081786A0(struct SceneHudState *);
void sub_08165254(struct SceneHudState *);
void sub_08178820(struct SceneHudState *hud)
{
    if (!hud->actionOwner)
        return;
    if (hud->controlMode == 7)
        return;
    if (hud->controlMode > 2)
        return;
    Hud_DrawDefaultHealthMeter(hud, 0, 4, 1);
    Hud_DrawDefaultEnergyMeter(hud, 0, 4, 2, hud->actionOwner->currentStatB, hud->actionOwner->maximumStatB);
    SceneHud_DrawSlotMeter(hud, 0, 9, 0x12);
    sub_08165020(hud);
    sub_08164DC0(hud);
    SceneHud_RefreshActionKeyframes(hud);
    sub_081786A0(hud);
    sub_08165254(hud);
}
