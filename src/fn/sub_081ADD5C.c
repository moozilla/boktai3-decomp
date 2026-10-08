#include "global.h"

struct A { u8 f[0xa8]; u8 g[4]; u8 a; };
void sub_082195E0(void *);
void sub_08214514(void *);

void sub_081ADD5C(struct A *p)
{
    sub_082195E0(p);
    if (p->a != 0)
        sub_08214514(p->g);
}
