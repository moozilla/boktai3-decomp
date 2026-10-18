#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#define SceneHud_DrawStatRatio Hud_DrawDefaultEnergyMeter
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_DrawDefaultEnergyMeter(void *, s32, s32, s32, s32, s32);
s32 Div(s32, s32);
void sub_0816F5B0(void);
void SceneHud_RefillActionStatB(struct SceneHudState *s)
{
    s32 complete;
    s32 step;
    s32 value;
    Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    complete = 0;
    step = Div(s->actionOwner->maximumStatB, 120);
    if (step <= 0)
        step = 1;
    value = s->actionOwner->currentStatB + step;
    if (value >= s->actionOwner->maximumStatB) {
        s->actionOwner->currentStatB = s->actionOwner->maximumStatB;
        complete = 1;
    } else {
        if (!s->actionTransferDelay) {
            Sound_PlaySong(0x259);
            s->actionTransferDelay++;
        }
        s->actionOwner->currentStatB = value;
    }
    Hud_DrawDefaultEnergyMeter(s, 1, 5, 4, s->actionOwner->currentStatB, s->actionOwner->maximumStatB);
    if (complete) {
        if (s->actionTransferDelay)
            Sound_StopSong(0x259);
        Hud_SetInteractionCallback(s, sub_0816F5B0, 1);
    }
}
