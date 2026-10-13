#include "palette_controller.h"
void Palette_ApplyFrame(struct PaletteController *);
s32 Palette_Update(struct PaletteController *p)
{
    struct PaletteTransition *t = &p->transition;
    p->current = 0;
    p->pending = 0;
    p->dirty = 0;
    Palette_ApplyFrame(p);
    p->previousBlend = p->blend;
    t->previousBrightness = t->brightness;
    p->previousState = gUnk_03005430.state;
}
