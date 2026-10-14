#include "modifier_list.h"

void Modifier_RemoveSelection(struct ModifierSelection *p, u32 n)
{
    p->mask &= ~(1 << n);
    p->count -= 1;
}
