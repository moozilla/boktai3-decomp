#include "global.h"
#include "scene_hud_display.h"
#include "player.h"

#define PauseMenu_RefreshStatusPanel sub_08169BE0
void Hud_DrawHealthMeter(struct SceneHudState *, s32, s32, s32, s32);
void sub_08164480(struct SceneHudState *, s32, s32, s32, s32, s32, s32);
void StatDisplay_DrawGauge(struct SceneHudState *, s32, s32);
void StatusPanel_ClearLowerRows(struct SceneHudState *);
void sub_08165020(struct SceneHudState *);
void sub_08164C40(struct SceneHudState *, s32);
void sub_08165254(struct SceneHudState *);
void StatusPanel_DrawFirstValue(struct SceneHudState *);
void StatusPanel_DrawSecondValue(struct SceneHudState *);
void PauseMenu_LoadAttributeIconFrame(struct SceneHudState *, void *, s32);
void PauseMenu_LoadFixedIconKeyframe(struct SceneHudState *, void *);
void PauseMenu_LoadEmptySlotFrame(struct SceneHudState *, void *);

void PauseMenu_RefreshStatusPanel(struct SceneHudState *p)
{
    s32 sel, icon;

    if (p->actionOwner && p->controlMode <= 2) {
        Hud_DrawHealthMeter(p, 0, 4, 1, 1);
        sub_08164480(p, 0, 4, 2, p->actionOwner->currentStatB, p->actionOwner->maximumStatB, 1);
        StatDisplay_DrawGauge(p, 3, 3);
        StatusPanel_ClearLowerRows(p);
        sub_08165020(p);
        sub_08164C40(p, p->controlMode);
        sub_08165254(p);
        StatusPanel_DrawFirstValue(p);
        StatusPanel_DrawSecondValue(p);
        if ((s8)p->actionOwner->thresholdMode >= 0) {
            sel = p->primarySelection;
            icon = p->primaryOptions[sel].icon;
            if (icon > 5) {
                PauseMenu_LoadAttributeIconFrame(p, &p->primaryWidgets[sel], icon);
            } else if (!p->actionOwner->actionEnabled) {
                if (p->primaryBlinkTimer == 0)
                    PauseMenu_LoadFixedIconKeyframe(p, &p->primaryWidgets[sel]);
                else if (p->primaryBlinkTimer == 30)
                    PauseMenu_LoadAttributeIconFrame(p, &p->primaryWidgets[sel], icon);
                if (++p->primaryBlinkTimer >= 60)
                    p->primaryBlinkTimer = 0;
            } else if (p->actionOwner->actionSelectionStamp) {
                if (++p->actionTransferDelay & 1)
                    PauseMenu_LoadAttributeIconFrame(p, &p->primaryWidgets[p->primarySelection],
                                                     p->primaryOptions[p->primarySelection].icon);
                else
                    PauseMenu_LoadEmptySlotFrame(p, &p->primaryWidgets[p->primarySelection]);
            } else {
                PauseMenu_LoadAttributeIconFrame(p, &p->primaryWidgets[sel], icon);
            }
        }
    }
}
