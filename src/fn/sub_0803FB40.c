#include "modifier_list.h"

s32 Modifier_ApplyToStat(struct ModifierSelection *, struct ModifierSelection *, s32, u32, u32);

s32 Modifier_ApplyPositiveKindThree(struct ModifierSelection *a, struct ModifierSelection *b, s32 c,
                                    u32 d)
{
    s32 r;
    if (c <= 0) {
        r = 0;
    } else {
        c = Modifier_ApplyToStat(a, b, c, d, 3);
        if (c > 999)
            c = 999;
        r = c;
    }
    return r;
}
