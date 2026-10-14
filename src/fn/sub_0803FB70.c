#include "modifier_list.h"

s32 Modifier_ApplyToStat(struct ModifierSelection *, struct ModifierSelection *, s32, u32, u32);

s32 Modifier_ApplyKindEight(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 8);
    if (c > 999)
        c = 999;
    return c;
}
