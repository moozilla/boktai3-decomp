#include "global.h"
#include "field_hud_widgets.h"
struct FieldHud *Hud_GetManager(void);
void FieldHud_InitializeNumber(struct FieldHudWidgets *, s32 *);
void FieldHud_InitializeGauge(struct FieldHudWidgets *, s32 **);
void Hud_SetValueSource(void *, u32);
void MeterDisplay_InitializeWidgets(struct FieldHudWidgets *);
void NumericDisplay_InitializeWidgets(struct FieldHudWidgets *, s32);
void FieldHud_InitializeWidgets(s32 *value, s32 **gauge, void *a, u16 b, s32 c)
{
    struct FieldHud *owner = Hud_GetManager();
    struct FieldHudWidgets *widgets = &owner->widgets;
    FieldHud_InitializeNumber(widgets, value);
    FieldHud_InitializeGauge(widgets, gauge);
    Hud_SetValueSource(a, b);
    MeterDisplay_InitializeWidgets(widgets);
    NumericDisplay_InitializeWidgets(widgets, c);
    owner->widgetsVisible = 1;
}
