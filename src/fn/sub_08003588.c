#include "palette_controller.h"

void sub_08003038(struct PaletteController *);

void sub_08003588(u32 a, u32 b, u32 c)
{
    struct PaletteController *p = gUnk_02000468;
    if (p != 0) {
        struct PaletteSlide *q = &p->slide;
        q->start = a;
        q->target = b;
        p->slide.value = a;
        q->time = 0;
        q->duration = c;
        p->flags |= 0xc;
        sub_08003038(p);
    }
}
