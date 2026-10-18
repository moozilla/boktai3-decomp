#include "global.h"
#include "field_hud_widgets.h"
void sub_081796D8(struct FieldHudWidgets *);
void FieldHud_UpdateGaugePalette(struct FieldHudWidgets *);
void FieldHud_RefreshGaugeIndex(struct FieldHudWidgets *);
void MeterDisplay_UpdateOwnerMeters(struct FieldHudWidgets *);
void sub_081793EC(struct FieldHudWidgets *);
void FieldHud_UpdateWidgets(struct FieldHud *p)
{
    struct FieldHudWidgets *widgets = &p->widgets;
    sub_081796D8(widgets);
    FieldHud_UpdateGaugePalette(widgets);
    FieldHud_RefreshGaugeIndex(widgets);
    MeterDisplay_UpdateOwnerMeters(widgets);
    sub_081793EC(widgets);
}
