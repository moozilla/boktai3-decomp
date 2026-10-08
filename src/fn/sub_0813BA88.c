#include "global.h"

struct T0813BA88 { u8 a; u8 b; u8 c; u8 d; };
extern const struct T0813BA88 gUnk_086108F8[];
void sub_08217EEC(u32 *, u32, u32);

void sub_0813BA88(u32 *p, u32 a, u32 i)
{
    const struct T0813BA88 *t = &gUnk_086108F8[i & 7];
    sub_08217EEC(p, a, t->a);
    if (t->b == 1)
        *p |= 4;
    else
        *p &= ~4;
    if (t->c == 1)
        *p |= 8;
    else
        *p &= ~8;
}
