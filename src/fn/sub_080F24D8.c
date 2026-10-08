#include "global.h"

u32 sub_0821BD48(void *);

struct S {
    u8 filler[0xb84];
    u8 a[0xd00 - 0xb84];
    u32 v;
    u8 f2[0xd80 - 0xd04];
    s32 n;
};

void sub_080F24D8(struct S *s)
{
    if (s->n > 7)
        s->v = (u16)sub_0821BD48(s->a);
}
