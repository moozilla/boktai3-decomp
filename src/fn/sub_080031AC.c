#include "palette_controller.h"
#define Palette_Update sub_080031AC
void sub_0800306C(struct PaletteController *);
s32 Palette_Update(struct PaletteController *p)
{
    struct PaletteTransition *t = &p->transition;
    p->current = 0;
    p->pending = 0;
    p->dirty = 0;
    sub_0800306C(p);
    p->previousBlend = p->blend;
    t->previousBrightness = t->brightness;
    p->previousState = gUnk_03005430.state;
}
