#include "global.h"
#include "color_fade_controller.h"

void sub_080036F8(struct ColorFadeChannel *p)
{
    p->level = 0x40 - ((p->time << 6) >> p->shift);
    if (p->time > (1 << p->shift)) {
        p->level = 0;
        if (p->mode == 2)
            p->mode = 0;
        else
            p->mode = 5;
    }
    p->time++;
}
