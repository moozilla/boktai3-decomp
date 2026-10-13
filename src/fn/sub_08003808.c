#include "global.h"
#include "color_fade_controller.h"

extern u32 gUnk_03004DA0;
extern u16 gUnk_030051C4;
extern u16 gUnk_03005214;
extern u32 gUnk_030051D4;
extern u16 gUnk_03005210;
extern u16 gUnk_030051DC;

void sub_08003808(struct ColorFadeController *s)
{
    struct ColorFadeChannel *p = &s->channels[0];
    if (p->mode != 0) {
        gUnk_03004DA0 = p->level;
        gUnk_030051C4 = p->start;
        gUnk_03005214 = p->color;
    }
    p = &s->channels[1];
    if (p->mode != 0) {
        gUnk_030051D4 = p->level;
        gUnk_03005210 = p->target;
        gUnk_030051DC = p->color;
    }
}
