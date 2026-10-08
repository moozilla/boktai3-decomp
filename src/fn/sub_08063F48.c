#include "global.h"

struct T { u32 a, b, c; };
extern const struct T gUnk_0824DF98;

struct S { u8 f[0x13C]; u16 a; u16 b; u32 c; };

void sub_08063F48(struct S *p, u16 i)
{
    struct T t = gUnk_0824DF98;
    p->a = i;
    p->b = 0;
    p->c = ((u32 *)&t)[p->a];
}
