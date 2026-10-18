// Tentative StatusPanel family and semantic names; based on static consumers.
#define StatusPanel_UpdateGaugePosition sub_0816A1E0
#include "global.h"
struct PanelGaugeValues { u8 unknown[0x428]; u16 current; u16 maximum; };
struct StatusPanel {
    u8 unknown0[0xA40];
    struct PanelGaugeValues *values;
    u8 unknownA44[0x3408 - 0xA44];
    u32 gaugeFlags;
    u8 unknown340C[0x3420 - 0x340C];
    s16 gaugeX;
    s16 gaugeY;
    u8 unknown3424[0x48EA - 0x3424];
    u16 gaugeIncrement;
};
s32 Div(s32, s32);
void sub_0816A1E0(struct StatusPanel *p)
{
    struct PanelGaugeValues *values = p->values;
    s32 amount = values->current + p->gaugeIncrement;
    s32 position = values->maximum;
    if (amount == 0)
        position = 0;
    else
        position = Div((amount - 1) * 79, position - 1) + 1;
    p->gaugeFlags &= ~1;
    p->gaugeX = position - 152;
    p->gaugeY = -40;
}
