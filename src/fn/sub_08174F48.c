#include "global.h"
#include "scene_input.h"
#include "scene_hud_display.h"
#include "player.h"
void ResultPanel_DrawCappedCounters(void *, s32, s32, s32);
void ResultPanel_DrawCappedSmallCounters(void *, s32, s32, s32);
void PauseMenu_HidePromptIcons(void *);
void Hud_DrawHealthMeter(void *, s32, s32, s32, s32);
void sub_08164480(void *, s32, s32, s32, s32, s32, s32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void InventoryGrid_UpdateModifierPreview(struct SceneHudState *p)
{
    u16 key = gPadState[0] & 4;
    if (key) {
        p->comparisonModifier = 1;
        ResultPanel_DrawCappedCounters(p, 1, 7, 8);
        ResultPanel_DrawCappedSmallCounters(p, 1, 7, 10);
        PauseMenu_HidePromptIcons(p);
    } else {
        p->comparisonModifier = key;
        Hud_DrawHealthMeter(p, 1, 8, 8, 1);
        sub_08164480(p, 1, 8, 10, p->actionOwner->currentStatB, p->actionOwner->maximumStatB, 1);
        SceneHud_SetPairedIcons(p, 1, -1);
    }
    p->comparisonIndicators[0].flags |= 1;
    p->comparisonIndicators[1].flags |= 1;
}
