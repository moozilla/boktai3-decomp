#include "global.h"
#define FieldHud_InitializeNumber sub_081799F8
#include "field_hud_widgets.h"
s32 Div(s32, s32);
void *ScriptProc_Start(u16, u16);
void sub_081799F8(struct FieldHudWidgets *p, s32 *source)
{
    s32 value;
    p->source = source;
    p->value = *source;
    value = p->value;
    if (p->value > 999)
        value = 999;
    p->displayedValue = value;
    p->numberColor = 0x1084;
    p->numberStep = Div(p->value * 3, 10);
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x6458) + 0x1B4, p->numberResource, 8);
}
