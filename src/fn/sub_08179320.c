#include "global.h"
#include "field_hud_widgets.h"
#include "render.h"
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);

void FieldHud_UpdateGaugePalette(struct FieldHudWidgets *p)
{
    u16 palette[16];
    s32 level, i;
    if (p->gaugeSource == NULL)
        level = 0;
    else
        level = Div(*(s32 *)p->gaugeSource, 0x898);
    if (p->fill > level)
        level = p->fill - 1;
    else if (p->fill < level)
        level = p->fill + 1;
    palette[0] = ((u16 *)p->paletteResource)[0xE0];
    palette[14] = ((u16 *)p->paletteResource)[0xEE];
    palette[15] = ((u16 *)p->paletteResource)[0xEF];
    for (i = 0; i <= 12; i++) {
        if (i < level)
            palette[i + 1] = ((u16 *)p->paletteResource)[0xE1 + i];
        else
            palette[i + 1] = ((u16 *)p->paletteResource)[0xEE];
    }
    CpuSet(palette, gBgPaletteBank14, 0x10);
    p->fill = level;
}
