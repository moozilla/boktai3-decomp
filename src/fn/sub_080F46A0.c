#include "global.h"

struct P { u8 f[0xcc8]; void *a; u8 g[0xcd0 - 0xccc]; void *b; };
void sub_0824923C(void *, void *);
void sub_080F24D8(void *);

void sub_080F46A0(struct P *p)
{
    if (p->a != 0) {
        sub_0824923C(p, p->a);
        if (p->b != 0)
            sub_0824923C(p, p->b);
        sub_080F24D8(p);
    }
}
