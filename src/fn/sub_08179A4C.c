#include "global.h"
#include "field_hud_widgets.h"
void *ScriptProc_Start(u16, u16);
void FieldHud_InitializeGauge(struct FieldHudWidgets *p, s32 **source)
{
    p->fill = 0;
    p->paletteResource = (u8 *)ScriptProc_Start(0x92B3, 0x6458) + 0x14;
    p->gaugeSource = source;
}
