#include "global.h"
#include "field_hud_widgets.h"
void FieldHud_UpdateNumberDisplay(struct FieldHudWidgets *);
void FieldHud_UpdateGaugePalette(struct FieldHudWidgets *);
void FieldHud_RefreshGaugeIndex(struct FieldHudWidgets *);
void MeterDisplay_UpdateOwnerMeters(struct FieldHudWidgets *);
void FieldHud_UpdateTimeDigits(struct FieldHudWidgets *);
void FieldHud_UpdateWidgets(struct FieldHud *p)
{
    struct FieldHudWidgets *widgets = &p->widgets;
    FieldHud_UpdateNumberDisplay(widgets);
    FieldHud_UpdateGaugePalette(widgets);
    FieldHud_RefreshGaugeIndex(widgets);
    MeterDisplay_UpdateOwnerMeters(widgets);
    FieldHud_UpdateTimeDigits(widgets);
}
