#include "palette_controller.h"

void sub_08002F2C(struct PaletteController *p)
{
    struct PaletteSlide *s = &p->slide;
    s->start = 0x40;
    s->target = 0x40;
    s->value = 0x40;
    s->time = 0;
    s->duration = 0;
}
