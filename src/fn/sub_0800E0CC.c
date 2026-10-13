#include "palette_set.h"

void sub_0800E0CC(struct PaletteSet *a, struct PaletteSetEntry *b)
{
    sub_08221110(b->blend, b->source[a->from], b->source[a->to], a->blend, 6);
    CpuSet(b->blend, b->colors, 0x10);
}
