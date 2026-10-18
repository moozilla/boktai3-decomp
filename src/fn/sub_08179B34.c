#include "global.h"
#define FieldHud_InitializeWidgets sub_08179B34
#define FieldHud_InitializeNumber  sub_081799F8
#define FieldHud_InitializeGauge   sub_08179A4C
#include "field_hud_widgets.h"
#define Hud_GetManager     sub_08177D7C
#define Hud_SetValueSource sub_081784EC
struct FieldHud *sub_08177D7C(void);
void FieldHud_InitializeNumber(struct FieldHudWidgets *, s32 *);
void FieldHud_InitializeGauge(struct FieldHudWidgets *, s32 **);
void sub_081784EC(void *, u32);
void sub_08179954(struct FieldHudWidgets *);
void sub_08179550(struct FieldHudWidgets *, s32);
void sub_08179B34(s32 *value, s32 **gauge, void *a, u16 b, s32 c)
{
    struct FieldHud *owner = sub_08177D7C();
    struct FieldHudWidgets *widgets = &owner->widgets;
    FieldHud_InitializeNumber(widgets, value);
    FieldHud_InitializeGauge(widgets, gauge);
    sub_081784EC(a, b);
    sub_08179954(widgets);
    sub_08179550(widgets, c);
    owner->widgetsVisible = 1;
}
