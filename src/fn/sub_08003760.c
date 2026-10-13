#include "global.h"
#include "color_fade_controller.h"

void sub_08003760(struct ColorFadeChannel *p)
{
    p->mode = 0;
    p->color = 0x1084;
    p->start = 0;
    p->level = 0x40;
    p->target = 0;
    p->time = 0;
    p->shift = 0;
}
