#include "modifier_list.h"

u32 Modifier_TestSelection(struct ModifierSelection *p, u32 n)
{
    return p->mask & (1 << n);
}
