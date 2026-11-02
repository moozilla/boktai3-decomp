#include "wooden_crate.h"

void WoodenCrate_SetFlags(struct WoodenCrate *p, u16 flags)
{
    p->crateFlags |= flags;
}
