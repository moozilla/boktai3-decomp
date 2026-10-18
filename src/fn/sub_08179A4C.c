#include "global.h"
#define FieldHud_InitializeGauge sub_08179A4C
#include "field_hud_widgets.h"
void *ScriptProc_Start(u16, u16);
void sub_08179A4C(struct FieldHudWidgets *p, s32 **source)
{
    p->fill = 0;
    p->paletteResource = (u8 *)ScriptProc_Start(0x92B3, 0x6458) + 0x14;
    p->gaugeSource = source;
}
