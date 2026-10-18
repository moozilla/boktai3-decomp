#define InventoryGrid_UpdateModifierPreview sub_08174F48
#include "global.h"
#include "scene_input.h"
#include "scene_hud_display.h"
#include "player.h"
void sub_08164158(void *, s32, s32, s32);
void sub_081641C4(void *, s32, s32, s32);
void PauseMenu_HidePromptIcons(void *);
void Hud_DrawHealthMeter(void *, s32, s32, s32, s32);
void sub_08164480(void *, s32, s32, s32, s32, s32, s32);
void sub_081660D0(void *, s32, s32);
void sub_08174F48(struct SceneHudState *p)
{
    u16 key = gUnk_03005260[0] & 4;
    if (key) {
        p->comparisonModifier = 1;
        sub_08164158(p, 1, 7, 8);
        sub_081641C4(p, 1, 7, 10);
        PauseMenu_HidePromptIcons(p);
    } else {
        p->comparisonModifier = key;
        Hud_DrawHealthMeter(p, 1, 8, 8, 1);
        sub_08164480(p, 1, 8, 10, p->actionOwner->currentStatB, p->actionOwner->maximumStatB, 1);
        sub_081660D0(p, 1, -1);
    }
    p->comparisonIndicators[0].flags |= 1;
    p->comparisonIndicators[1].flags |= 1;
}
