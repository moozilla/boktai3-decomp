#include "palette_controller.h"

void sub_08002D58(struct PaletteController *, struct PaletteTransition *);

void sub_08002E28(struct PaletteController *p, struct PaletteTransition *t)
{
    sub_08002D58(p, t);
    t->level++;
    if (t->level > 0x3f)
        t->level = 0x40;
}
