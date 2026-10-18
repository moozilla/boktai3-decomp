#include "global.h"
#define FieldHud_UpdateWidgets      sub_08179B04
#define FieldHud_UpdateGaugePalette sub_08179320
#define FieldHud_RefreshGaugeIndex  sub_081793D0
#include "field_hud_widgets.h"
void sub_081796D8(struct FieldHudWidgets *);
void FieldHud_UpdateGaugePalette(struct FieldHudWidgets *);
void FieldHud_RefreshGaugeIndex(struct FieldHudWidgets *);
void sub_08179A74(struct FieldHudWidgets *);
void sub_081793EC(struct FieldHudWidgets *);
void sub_08179B04(struct FieldHud *p)
{
    struct FieldHudWidgets *widgets = &p->widgets;
    sub_081796D8(widgets);
    FieldHud_UpdateGaugePalette(widgets);
    FieldHud_RefreshGaugeIndex(widgets);
    sub_08179A74(widgets);
    sub_081793EC(widgets);
}
