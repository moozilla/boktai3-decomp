#include "global.h"
#include "field_hud_widgets.h"
void Hud_DrawDefaultEnergyMeter(struct FieldHud *, s32, s32, s32, s32, s32);
void Hud_DrawWarningMeter(struct FieldHud *, s32, s32, s32);
void FieldHud_UpdateWidgets(struct FieldHud *);
void FieldHud_RefreshModeEight(struct FieldHud *p)
{
    if (p->mode == 8) {
        Hud_DrawDefaultEnergyMeter(p, 0, 3, 2, *p->valueSource, p->valueMode);
        Hud_DrawWarningMeter(p, 0, 0x12, 1);
        FieldHud_UpdateWidgets(p);
    }
}
