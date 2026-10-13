#include "scanline_presentation.h"
s32 Div(s32, s32);
void sub_0800D9FC(struct ScanlinePresentation *s)
{
    u32 k;
    if (s->mode == 0)
        gUnk_03004DA0 = Div(s->phase << 6, s->duration);
    s->phase--;
    k = 0;
    if (s->phase < 0) {
        s->phase = k;
        s->fade = (void *)k;
        if (s->script != 0)
            Script_Call(s->script, 0);
        Task_Kill(s);
    }
}
