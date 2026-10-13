#include "palette_set.h"

void sub_0800E10C(struct PaletteSet *a, struct PaletteSetEntry *b)
{
    sub_08221110(b->colors, b->blend, b->overlay, a->overlay, 6);
}
