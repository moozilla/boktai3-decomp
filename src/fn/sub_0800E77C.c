#include "global.h"

struct S {
    u8 f[0x28];
    u16 fl;
};
u32 sub_08228D7C(void);
void sub_082286E0(s32 *, s32 *, s32 *, u32);
void NumericDisplay_DrawPair(struct S *, s32, s32);
void NumericDisplay_DrawStatusValues(struct S *);
void sub_0800E5B8(struct S *);

void sub_0800E77C(struct S *s)
{
    s32 a, b, c;
    sub_082286E0(&a, &b, &c, sub_08228D7C());
    if (s->fl & 1)
        NumericDisplay_DrawPair(s, b, c);
    if (s->fl & 2)
        NumericDisplay_DrawStatusValues(s);
    if (s->fl & 4)
        sub_0800E5B8(s);
    s->fl = 0;
}
