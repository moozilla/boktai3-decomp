#include "global.h"
#include "scene_hud_display.h"
void Hud_DrawDefaultEnergyMeter(struct SceneHudState *, s32, s32, s32, s32, s32);
void Hud_DrawWarningMeter(struct SceneHudState *, s32, s32, s32);
void FieldHud_UpdateWidgets(struct SceneHudState *);
void FieldHud_RefreshModeEight(struct SceneHudState *p)
{
    if (p->controlMode == 8) {
        Hud_DrawDefaultEnergyMeter(p, 0, 3, 2, *p->fieldValueSource, p->fieldValueMode);
        Hud_DrawWarningMeter(p, 0, 0x12, 1);
        FieldHud_UpdateWidgets(p);
    }
}
