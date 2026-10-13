#include "global.h"
#include "color_fade_controller.h"

void sub_08003734(struct ColorFadeChannel *p)
{
    p->level = (p->time << 6) >> p->shift;
    if (p->time > (1 << p->shift)) {
        p->level = 0x40;
        p->mode = 0;
    }
    p->time++;
}
