#include "palette_controller.h"

s32 Div(s32, s32);

void sub_08002EE4(struct PaletteController *p)
{
    struct PaletteSlide *s = &p->slide;
    s32 r;
    s32 time = s->time;
    if (time == 0) {
        r = s->start;
    } else {
        s32 duration = s->duration;
        if (duration == 0 || time >= duration) {
            p->slide.value = s->target;
            goto end;
        }
        r = Div(s->start * (duration - time) + time * s->target, duration);
    }
    p->slide.value = r;
    s->time++;
end:
    p->blend = s->value;
}
