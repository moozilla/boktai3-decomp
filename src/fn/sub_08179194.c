#include "global.h"
#define FieldHud_RefreshModeEight sub_08179194
#include "field_hud_widgets.h"
#define FieldHud_UpdateWidgets sub_08179B04
void Hud_DrawDefaultEnergyMeter(struct FieldHud *, s32, s32, s32, s32, s32);
void sub_08164864(struct FieldHud *, s32, s32, s32);
void sub_08179B04(struct FieldHud *);
void sub_08179194(struct FieldHud *p)
{
    if (p->mode == 8) {
        Hud_DrawDefaultEnergyMeter(p, 0, 3, 2, *p->valueSource, p->valueMode);
        sub_08164864(p, 0, 0x12, 1);
        sub_08179B04(p);
    }
}
