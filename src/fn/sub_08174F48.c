#include "global.h"
#include "scene_input.h"
#include "scene_hud_display.h"
#include "player.h"
void Hud_DrawHealthRatio(void *, s32, s32, s32);
void Hud_DrawEnergyRatio(void *, s32, s32, s32);
void PauseMenu_HidePromptIcons(void *);
void Hud_DrawHealthMeter(void *, s32, s32, s32, s32);
void Hud_DrawEnergyMeter(void *, s32, s32, s32, s32, s32, s32);
void PauseMenu_SetPairedIcons(void *, s32, s32);
void InventoryGrid_UpdateModifierPreview(struct SceneHudState *p)
{
    u16 key = gPadState[0] & 4;
    if (key) {
        p->comparisonModifier = 1;
        Hud_DrawHealthRatio(p, 1, 7, 8);
        Hud_DrawEnergyRatio(p, 1, 7, 10);
        PauseMenu_HidePromptIcons(p);
    } else {
        p->comparisonModifier = key;
        Hud_DrawHealthMeter(p, 1, 8, 8, 1);
        Hud_DrawEnergyMeter(p, 1, 8, 10, p->actionOwner->currentStatB, p->actionOwner->maximumStatB, 1);
        PauseMenu_SetPairedIcons(p, 1, -1);
    }
    p->comparisonIndicators[0].flags |= 1;
    p->comparisonIndicators[1].flags |= 1;
}
