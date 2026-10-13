#include "palette_controller.h"

void sub_08227950(u16 *, u16 *);

void sub_08002C38(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 x[2];
    u16 *y = &x[1];
    sub_08227950(x, y);
    t->from = p->colorSets[PALETTE_SET_VARIANT][x[0]];
    t->to = p->colorSets[PALETTE_SET_VARIANT][*y];
}
