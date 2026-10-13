#include "scanline_presentation.h"
s32 Div(s32, s32);
void sub_0800D9A0(struct ScanlinePresentation *s)
{
    if (s->mode == 0)
        gUnk_03004DA0 = Div(s->phase << 6, s->duration);
    s->phase++;
    if (s->phase > s->duration) {
        s->phase = s->duration;
        s->fade = 0;
    }
}
