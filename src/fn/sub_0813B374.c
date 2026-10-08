#include "global.h"

struct S0813B374 { u8 filler[0xB3C]; void *v; };
struct C { u32 b; void *c; };
struct D { u32 a; struct C c; };
void sub_0821AD08(void *, void *);

void sub_0813B374(struct S0813B374 *p, u32 a)
{
    struct D d;
    void *r = p->v;
    if (r != 0) {
        struct C *q; u32 m;
        m = 0xFFFF0000; d.c.b = (d.c.b & m) | 1;
        d.a = a;
        q = &d.c;
        q->c = &d;
        sub_0821AD08(r, q);
    }
}
